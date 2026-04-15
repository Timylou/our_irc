/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:43:21 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/15 19:33:13 by yel-mens         ###   ########.fr       */
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
				continue;
			}
			int	check = readMessage(client);
			if (check == 1)// read client message
			{
				std::cout << client->getUsername() << i << " : " << client->getBuffer();
				doCmd(client);
				//broadcast(client, client->getBuffer());
				client->setBuffer("\0");
			}
			else if (check == -1)
			{
				removeClient(client, i);
				i--;
			}
		}
	}
}

/*********************** READ ************************/

int	Server::readMessage(Client *client) { // int 3 cas : 0 == send rien 1 == send message -1 == supp client

	int		bytes;
	char	buffer[BUFFER_SIZE + 1];

	do {
		bytes = recv(client->getSocket(), buffer, BUFFER_SIZE, MSG_DONTWAIT);
		if (bytes == 0)
			return (-1);
		else if (bytes == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				errno = 0;
				return (0);
			}
			return (-1);
		}
		if (bytes > 2 && buffer[bytes - 2] != '\r')
			buffer[bytes - 1] = 0;
		else
			buffer[bytes] = 0;
		client->getBuffer().append(buffer);
	} while (bytes >= 0 && buffer[bytes - 2] != '\r' && buffer[bytes - 1] != '\n');
	if (client->getBuffer().find("\r\n") != std::string::npos)
		return 1;
	return 0;
}

/*********************** GETTERS *********************/

int				Server::getSocket(void) const {return (this->_listenSocket);}
unsigned short	Server::getPort(void) const {return (this->_port);}
std::string		Server::getPassword(void) const {return (this->_password);}

/*********************** CHANNEL *********************/

bool			Server::findChannel(const std::string &channelName) {return (_channels.find(channelName) != _channels.end());}
void			Server::addChannel(const std::string &channelName) {_channels[channelName] = new Channel(channelName);}

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
	for (std::map<std::string, Channel *>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel)
		itChannel->second->removeClient(client);
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

	for (std::map<std::string, Channel *>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel)
	{
		if (!itChannel->second->findClient(client))
			continue;
		channelClients = itChannel->second->getClients();
		for (std::map<Client *, bool>::iterator itClient = channelClients.begin(); itClient != channelClients.end(); ++itClient)
		{
			std::vector<Client *>::iterator	itVector = std::find(clientList.begin(), clientList.end(), itClient->first);
			if (itVector == clientList.end() && itClient->first != client)
				clientList.push_back(itClient->first);
		}
	}
	return (clientList);
}

/********************* COMMANDS **********************/

void	Server::doCmd(Client *client)
{
	IRCMessage	*message = IRCparsing(client->getBuffer());

	try
	{
		if (message->command == "JOIN")
			this->join(client, message);
		else if (message->command == "PRIVMSG")
			this->privmsg(client, message);
		else
			throw std::runtime_error("command not found\r\n");
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

void	Server::join(Client *client, IRCMessage *message)
{
	std::string	channelName;

	if (!message || message->params.empty() || message->params[0].empty())
		throw (std::runtime_error("Join command needs parameter\n"));
	channelName = message->params[0];
	if (channelName[0] != '#')
		throw (std::runtime_error("Channel name always begind with #\n"));
	// check if channels exist
	if (this->findChannel(channelName))
	{	// TODO : GERER LES PASSWORD ET RAGARDER SI LE CHANNEL EST EN INVITE ONLY MODE
		if (_channels[channelName]->getMode(MODE_K))
		{
			if (message->params.size() < 2)
				throw (std::runtime_error("This channel is protected by a password\n"));
			else if (message->params[1] != _channels[channelName]->getPassword())
				throw (std::runtime_error("Password Incorrect\n"));
		}
		if (_channels[channelName]->getMode(MODE_I))
			throw (std::runtime_error("This channel is on invite-only mode\n"));
		_channels[channelName]->addClient(client);
	}
	else
	{
		this->addChannel(channelName);
		this->_channels[channelName]->addClient(client);
	}
		// 🔹 message JOIN (format IRC)
	std::string msgJoin = ":" + client->getNickname() + " JOIN " + channelName;
	this->_channels[channelName]->Broadcast(client, msgJoin);
}

void	Server::privmsg(Client *client, IRCMessage *message)
{
	if (!message || message->params.empty() || message->params[0].empty())
		throw (std::runtime_error("Privmsg command needs parameter\n"));
	std::string chan_user = message->params[0];
	if (chan_user[0] == '#'){
		// gere channel
		if (!findChannel(chan_user))
			throw (std::runtime_error("Privmsg : need an existant channel\n"));
		Channel	*channel = _channels[chan_user];
		if (!channel->findClient(client))
			throw (std::runtime_error("Privmsg : Client is not in channel\n"));
		std::string msg = ":" + client->getNickname() + " " + message->params[1];
		channel->Broadcast(client, msg);
		std::cout << msg << std::endl;
	}
	// else{
		// gere user
	// }
}
