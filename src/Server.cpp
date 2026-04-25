/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:43:21 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/25 19:37:15 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

/******************** SIGNAL HANDLER ********************/

volatile sig_atomic_t g_running = 1;

void handleSignal(int signal){
	g_running = 0;
}

/******************** CONSTRUCTORS ********************/

void	serverInit(Server *serv)
{
	int			opt;		// config for the address and the port
	sockaddr_in	serverAddr;	// struct of the address server
	pollfd		pfd;		// struct for poll to listen the listenSocket

	// Config listen socket
	opt = 1;
	if (setsockopt(serv->getSocket(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Error : Cannot config socket");

	// Config server address
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;			// IPV4
	serverAddr.sin_addr.s_addr = INADDR_ANY;	// All kind of IP
	serverAddr.sin_port = htons(serv->getPort());

	// Link server address and listen socket
	if (bind(serv->getSocket(), (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		throw (std::runtime_error("Error linking socket and address : " + std::string(strerror(errno))));

	// Set socket to listen clients
	if (listen(serv->getSocket(), MAX_CLIENTS) < 0)
		throw (std::runtime_error("Error : Cannot set the socket to listen"));

	// Add listenSocket's pollfd
	serv->addClient(serv->getSocket());	// clients[0] == listenSocket
}

Server::Server(std::string port, std::string password)
{
	// set port
	if (port.empty())
		throw std::runtime_error("Empty port string");
	for (size_t i = 0; i < port.size(); i++)
	{
		if (!std::isdigit(port[i]))
			throw std::runtime_error("Port can only be digits");
	}
	int	port_n;
	std::istringstream(port) >> port_n;
	if (port_n <= 0 || port_n > 65535)
		throw std::runtime_error("Invalid port range");
	this->_port = port_n;

	// set password
	if (password.empty())
		throw std::runtime_error("Empty password string");
	if (password.size() < 5 || password.size() > 20)
		throw std::runtime_error("Password lenght must be contained between 5 and 20 characters.");
	this->_password = password;

	// Open listen socket
	this->_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listenSocket < 0)
		throw (std::runtime_error("Error : Cannot open listen socket"));
	std::signal(SIGINT, handleSignal);
	serverInit(this);
}

/********************* DESTRUCTOR ********************/

Server::~Server(void) {
	std::string shutdownMsg = "\nServer shutting down\r\n"; // \r\n norm IRC
	for (size_t i = 1; i < _pfd.size(); ++i){
		send(_pfd[i].fd, shutdownMsg.c_str(), shutdownMsg.size(), 0); // sender to client
		removeClient(_clients.find(_pfd[i].fd)->second, i);
		i--;
	}
	removeClient(_clients.find(_pfd[0].fd)->second, 0);
	for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	std::cout << std::endl << "Signal received, shutting down the server..." << std::endl;
}

/************************* RUN ***********************/

void	Server::run(void)
{
	int		clientSocket;
	Client	*client;

	while (g_running)
	{
		if (poll(this->_pfd.data(), this->_pfd.size(), -1) < 0)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll error");
		}

		for (size_t i = 0; i < this->_pfd.size(); ++i)
		{
			if (!(this->_pfd[i].revents & POLLIN)) // If the client has nothing to say skip it
			{
				this->_pfd[i].revents = 0;
				continue;
			}

			if (this->_pfd[i].fd == this->getSocket())
			{
				clientSocket = accept(this->getSocket(), NULL, NULL);
				if (clientSocket >= 0)
					this->addClient(clientSocket);
				continue;
			}
			client = _clients.find(this->_pfd[i].fd)->second;

			int check = readMessage(client);
			if (check == -1)
			{
				removeClient(client, i);
				i--;
				continue;
			}
			if (check == 1) // read client message
			{
				std::string message;

				while (joinMessage(client->getBuffer(), message))
				{
					static int counter = 0;
					std::cout << "[doCmd #" << ++counter << "] " << message << std::endl;
					std::cout << "[debug] : " << "client "<< client->getSocket() << " " << client->getNickname() <<" : " << message << std::endl;
					doCmd(client, message);
				}
			}
		}
	}
}

/*********************** READ ************************/

int	Server::readMessage(Client *client) { // int 3 cas : 0 == send rien 1 == send message -1 == supp client

	int		bytes;
	char	buffer[BUFFER_SIZE];

	while (true) {
		bytes = recv(client->getSocket(), buffer, BUFFER_SIZE, MSG_DONTWAIT);
		if (bytes == 0)
			return (-1);
		else if (bytes == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			return (-1);
		}
		client->getBuffer().append(buffer, bytes);
	}
	if (client->getBuffer().find("\r\n") != std::string::npos)
		return 1;
	return 0;
}

/*********************** CHANNEL *********************/

bool	Server::findChannel(const std::string &channelName) {return (_channels.find(channelName) != _channels.end());}
void	Server::addChannel(const std::string &channelName) {_channels[channelName] = new Channel(channelName);}

/*********************** CLIENTS *********************/

void	Server::addClient(int socket)
{
	if (_clients.find(socket) != _clients.end())
		return;

	pollfd	pfd;
	Client	*client = new Client(socket);

	pfd.fd = socket;
	pfd.events = POLLIN;			// prevent poll to read this pollfd
	pfd.revents = 0;				// revents will be 1 when _clients try to connect
	this->_pfd.push_back(pfd);		// add the client's pollfd
	_clients.insert(_clients.begin(), std::make_pair(socket, client)); // add a new client

}

void	Server::removeClient(Client *client, int numClient)
{
	close(client->getSocket()); // close pollfd
	_pfd.erase(this->_pfd.begin() + numClient); //  supprimer le pollfd du vecteur
	_clients.erase(client->getSocket()); // delete le client de la map
	for (std::map<std::string, Channel *>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel)
		itChannel->second->removeClient(client);
	delete client;
}

Client* Server::getClient(std::string nickname) {
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second->getNickname() == nickname)
			return (it->second);
	}
	return (NULL);
}

/********************* COMMANDS **********************/

void	Server::doCmd(Client *client, std::string line)
{
	IRCMessage	*message = IRCparsing(line);

	try
	{
		if (message->command == "CAP" && !client->getCap())
		{
			client->setCap(true);
			send(client->getSocket(), CAP_ANS, sizeof(CAP_ANS) - 1, MSG_DONTWAIT);
		}
		else if (message->command == "NICK")
			handleNICK(*this, client, message);
		else if (message->command == "USER")
			handleUSER(*this, client, message);
		else if (message->command == "PASS")
			handlePASS(*this, client, message);
		else if (message->command == "JOIN")
			handleJOIN(*this, client, message);
		else if (message->command == "PRIVMSG")
			handlePRIVMSG(*this, client, message);
		else if (!client->getStatus())
			throw (std::runtime_error(UNREG));
		else
			throw std::runtime_error(NOT_FOUND);
	}
	catch (std::exception &e)
	{
		std::string	msg(e.what());
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			send(client->getSocket(), msg.c_str(), msg.length(), MSG_DONTWAIT);
		errno = 0;
	}
	delete (message);
}

