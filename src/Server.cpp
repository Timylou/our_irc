/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:43:21 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/14 16:12:10 by amairia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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

Server::Server(void)	:	_port(6667), _password("miaou")
{
	// Open listen socket
	this->_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listenSocket < 0)
		throw (std::runtime_error("Error : Cannot open listen socket"));
	// handlers for SIGINT
	std::signal(SIGINT, handleSignal);
	serverInit(this);
}

Server::Server(unsigned short port, std::string password)	:	_port(port), _password(password)
{
	// Open listen socket
	this->_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listenSocket < 0)
		throw (std::runtime_error("Error : Cannot open listen socket"));
	// handlers for SIGINT
	std::signal(SIGINT, handleSignal);
	serverInit(this);
}

/********************* DESTRUCTOR ********************/

Server::~Server(void) {
	std::string shutdownMsg = "\nServer shutting down\r\n"; // \r\n norm IRC
	for (size_t i = 1; i < _pfd.size(); ++i){
		send(_pfd[i].fd, shutdownMsg.c_str(), shutdownMsg.size(), 0); // sender to client
		//close(_clients[i].fd);
		removeClient(_clients.find(_pfd[i].fd)->second, i);
		i--;
	}
	removeClient(_clients.find(_pfd[0].fd)->second, 0);
	std::cout << std::endl << "Signal received, shutting down the server..." << std::endl;
}

/************************* RUN ***********************/

void	Server::run(void)
{
	int		clientSocket;
	Client	*client;

	while (g_running) // global for SIGINT SIGTERM
	{
		size_t ret = poll(_pfd.data(), _pfd.size(), -1);
		if (ret < 0)
		{
			if (errno == EINTR) // Interrupted by a signal (Ctrl+C)
				continue;
		if (poll(this->_pfd.data(), this->_pfd.size(), -1) < 0)
			throw std::runtime_error("poll error");
		}

		for (size_t i = 0; i < this->_pfd.size(); ++i)
		{
			if (!(this->_pfd[i].revents & POLLIN)) // If the client has nothing to say skip it
				continue;

			client = _clients.find(this->_pfd[i].fd)->second;
			// 🔸 New client
			if (this->_pfd[i].fd == this->getSocket())
			{
				clientSocket = accept(this->getSocket(), NULL, NULL);
				if (clientSocket >= 0)
					this->addClient(clientSocket);
			}
			else if (readMessage(client))// read client message
			{
				std::cerr << client->getUsername() << i << " : " << client->getBuffer();
				// doCmd(client);
				broadcast(client, client->getBuffer());
			}
			else
			{
				removeClient(client, i);
				i--;
			}
		}
	}
}

/*********************** READ ************************/

bool	Server::readMessage(Client *client) {

	int		bytes;
	char	buffer[BUFFER_SIZE + 1];

	client->setBuffer("\0");
	do {
		bytes = recv(client->getSocket(), buffer, BUFFER_SIZE, MSG_DONTWAIT);
		std::cout << bytes << std::endl;
		if (bytes <= 0) {
			broadcast(client, "QUIT : leaving chat\r\n");
			return (false);
		}
		buffer[bytes] = 0;
		client->getBuffer().append(buffer);
	} while (bytes == BUFFER_SIZE && buffer[bytes - 1] != '\n' && buffer[bytes - 2] != '\r');
	return (client->getBuffer().find("\r\n") != std::string::npos);
}

// void	doCmd(Client *client) {
// 	// parsing commande "CMD blabla"

// }

/*********************** GETTERS *********************/

int				Server::getSocket(void) const {return (this->_listenSocket);}
unsigned short	Server::getPort(void) const {return (this->_port);}
std::string		Server::getPassword(void) const {return (this->_password);}

/*********************** CLIENTS *********************/

void	Server::addClient(int socket)
{
	pollfd	pfd;
	Client	*client = new Client(socket);

	pfd.fd = socket;
	pfd.events = POLLIN;			// prevent poll to read this pollfd
	pfd.revents = 0;				// revents will be 1 when _clients try to connect
	this->_pfd.push_back(pfd);		// add the client's pollfd
	_clients.insert(_clients.begin(), std::make_pair(socket, client)); // add a new client

	/* A ENLEVER !!!!!!!!
	if (socket != getSocket())
	{
		this->_channels.push_back(new Channel("miaou")); // A ENLEVER !!!!!!!!
		_channels[0]->addClient(client); // A ENLEVER !!!!!!!!
	} A ENLEVER !!!!!!!!*/
}

void	Server::removeClient(Client *client, int numClient)
{
	close(client->getSocket()); // close pollfd
	_pfd.erase(this->_pfd.begin() + numClient); //  supprimer le pollfd du vecteur
	_clients.erase(client->getSocket()); // delete le client de la map
	for (std::vector<Channel *>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel)
		(*itChannel)->removeClient(client);
	delete client;
}

/********************* BROADCAST *********************/

void	Server::broadcast(Client *client, std::string message)
{
	std::vector<Client *>	clientList = getListenningClients(client);
	for (std::vector<Client *>::iterator it = clientList.begin(); it != clientList.end(); ++it)
		send((*it)->getSocket(), message.c_str(), message.length(), MSG_DONTWAIT);
}

std::vector<Client *>	Server::getListenningClients(Client *client)
{
	std::vector<Client *>		clientList;
	std::map<Client *, bool>	channelClients;

	for (std::vector<Channel *>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel)
	{
		if (!(*itChannel)->findClient(client))
			continue;
		channelClients = (*itChannel)->getClients();
		for (std::map<Client *, bool>::iterator itClient = channelClients.begin(); itClient != channelClients.end(); ++itClient)
		{
			std::vector<Client *>::iterator	itVector = std::find(clientList.begin(), clientList.end(), itClient->first);
			if (itVector == clientList.end() && itClient->first != client)
				clientList.push_back(itClient->first);
		}
	}
	return (clientList);
}
