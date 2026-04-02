/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:43:21 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/02 21:16:08 by amairia          ###   ########.fr       */
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
	for (size_t i = 1; i < _clients.size(); ++i){
		send(_clients[i].fd, shutdownMsg.c_str(), shutdownMsg.size(), 0); // sender to client
		close(_clients[i].fd);
	}
	_clients.clear();
	close(_listenSocket);
	std::cout << std::endl << "Signal received, shutting down the server..." << std::endl;
}

/************************* RUN ***********************/

void	Server::run(void)
{
	char	buffer[BUFFER_SIZE];
	int		clientSocket;
	int		bytes;

	while (g_running) // global for SIGINT SIGTERM
	{
		// set for check ctrlD
		pollfd stdinPoll;
		stdinPoll.fd = STDIN_FILENO;
		stdinPoll.events = POLLIN;
		stdinPoll.revents = 0;
		std::vector<pollfd> fds = this->_clients;
		fds.push_back(stdinPoll);

		//OLD VERSION
	//	if (poll(this->_clients.data(), this->_clients.size(), -1) < 0)
	//		throw std::runtime_error("poll error");
		// NEW VERSION
		size_t ret = poll(fds.data(), fds.size(), -1);
		if (ret < 0)
		{
			if (errno == EINTR) // Interrupted by a signal (Ctrl+C)
				continue;
			throw std::runtime_error("poll error");
		}

		// check ctrlD
		pollfd& stdinFd = fds.back();
		if (stdinFd.revents & POLLIN){
			char tmp[2];
			if (!fgets(tmp, sizeof(tmp), stdin)) {
				std::cout << std::endl << "EOF detected" << std::endl;
				g_running = 0;
				continue;
			}
		}

		for (size_t i = 0; i < this->_clients.size(); ++i)
		{
			if (!(fds[i].revents & POLLIN)) // If the client has nothing to say skip it
				continue;

			// 🔸 New client
			if (this->_clients[i].fd == this->getSocket())
			{
				clientSocket = accept(this->getSocket(), NULL, NULL);
				if (clientSocket >= 0)
					this->addClient(clientSocket);
			}
			else // read client message
			{
				bytes = recv(this->_clients[i].fd, buffer, BUFFER_SIZE - 1, 0);
				if (bytes <= 0)
				{
					this->removeClient(i);
					--i;
				}
				else
				{
					buffer[bytes] = '\0';
					std::cout << "Client " << this->_clients[i].fd << " : " << buffer;
					send(this->_clients[i].fd, buffer, bytes, 0); // TODO : Broadcast
				}
			}
		}
	}
}

/*********************** GETTERS *********************/

int				Server::getSocket(void) const {return (this->_listenSocket);}
unsigned short	Server::getPort(void) const {return (this->_port);}
std::string		Server::getPassword(void) const {return (this->_password);}

/*********************** CLIENTS *********************/

void	Server::addClient(int socket)
{
	pollfd	pfd;

	pfd.fd = socket;
	pfd.events = POLLIN;			// prevent poll to read this pollfd
	pfd.revents = 0;				// revents will be 1 when _clients try to connect
	this->_clients.push_back(pfd);	// add the client in the client's array
}

void	Server::removeClient(int numClient)
{
	std::cout << "Client "<< this->_clients[numClient].fd << " disconnected" << std::endl;
	close(this->_clients[numClient].fd);
	this->_clients.erase(this->_clients.begin() + numClient);
}
