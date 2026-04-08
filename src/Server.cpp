/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:43:21 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/08 19:39:12 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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
	serverInit(this);
}

Server::Server(unsigned short port, std::string password)	:	_port(port), _password(password)
{
	// Open listen socket
	this->_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_listenSocket < 0)
		throw (std::runtime_error("Error : Cannot open listen socket"));
	serverInit(this);
}

/********************* DESTRUCTOR ********************/

Server::~Server(void)
{

}

/************************* RUN ***********************/

void	Server::run(void)
{
	int		clientSocket;
	Client	*client;

	while (true)
	{
		if (poll(this->_pfd.data(), this->_pfd.size(), -1) < 0)
			throw std::runtime_error("poll error");

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
				broadcast(client);
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
		if (bytes <= 0) {
			client->setBuffer("QUIT : leaving chat\r\n"); // TODO: add client username in quit message uwu
			broadcast(client);
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
}

void	Server::removeClient(Client *client, int numClient)
{
	close(client->getSocket()); // close pollfd
	_pfd.erase(this->_pfd.begin() + numClient); //  supprimer le pollfd du vecteur
	_clients.erase(client->getSocket()); // delete le client de la map
	delete client;
}

/********************* BROADCAST *********************/

void	Server::broadcast(Client *client)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first != client->getSocket() && it->first != this->getSocket())
			send(it->second->getSocket(), client->getBuffer().c_str(), client->getBuffer().length(), MSG_DONTWAIT);
	}
}
