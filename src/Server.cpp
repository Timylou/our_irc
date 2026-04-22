/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:43:21 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/22 12:52:21 by julifern         ###   ########.fr       */
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
					std::cout << client->getUsername() << i << " : " << message << std::endl;
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
		// if (bytes > 2 && buffer[bytes - 2] != '\r' && buffer[bytes - 1] == '\n')
		// 	buffer[bytes - 2] = 0;
		// else
		// 	buffer[bytes] = 0;
		std::cout << "{{" << client->getBuffer() << "}}" << std::endl;
		client->getBuffer().append(buffer, bytes);
	}
	// while (bytes >= 0 && buffer[bytes - 2] != '\r' && buffer[bytes - 1] != '\n');
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
	std::string msg = message;
	if (msg.size() < 2 || msg.substr(msg.size() - 2) != "\r\n")
		msg += "\r\n";
	std::cout << "[[" << msg << "]]" << std::endl;
	for (std::vector<Client *>::iterator it = clientList.begin(); it != clientList.end(); ++it)
	{
		if (send((*it)->getSocket(), msg.c_str(), msg.length(), MSG_DONTWAIT) < 0)
			std::cerr << "send failed to client fd " << (*it)->getSocket() << std::endl;
	}
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
			this->nick(client, message);
		else if (message->command == "USER")
			this->user(client, message);
		else if (message->command == "PASS")
			this->pass(client, message);
		else if (!client->getStatus())
			throw (std::runtime_error(UNREG));
		else if (message->command == "JOIN")
			this->join(client, message);
		else if (message->command == "PRIVMSG")
			this->privmsg(client, message);
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
		this->_channels[channelName]->promoteClient(client);
	}
	// 🔹 message JOIN (format IRC)
	std::string msgJoin = ":" + client->getNickname() + " JOIN " + channelName + "\r\n";
	this->_channels[channelName]->Broadcast(client, msgJoin);
	msgJoin = ":server 331 " + client->getNickname() + " " + channelName + " :No topic is set\r\n";
	if (!this->_channels[channelName]->getTopic().empty())
		msgJoin = ":server 332 " + client->getNickname() + " " + channelName + " :topic\r\n";
	msgJoin += ":server 353 " + client->getNickname() + " = " + channelName + " :" + _channels[channelName]->getStringClient() + "\r\n";
	msgJoin += ":server 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
	send(client->getSocket(), msgJoin.c_str(), msgJoin.length(), MSG_DONTWAIT);
}

void	Server::privmsg(Client *client, IRCMessage *message)
{
	std::cout << "!!!!!!!!!!privmsg!!!!!!!!!!" << std::endl;
	if (!message || message->params.empty() || message->params[0].empty())
		throw (std::runtime_error("Privmsg command needs parameter\n"));
	std::string chan_user = message->params[0];
	if (chan_user[0] == '#'){
		// gere channel
		if (!findChannel(chan_user))
			throw (std::runtime_error("Privmsg : need an existant channel\r\n"));
		Channel	*channel = _channels[chan_user];
		if (!channel->findClient(client))
			throw (std::runtime_error("Privmsg : Client is not in channel\r\n"));
		// std::string msg = channel->getName() + " :" + client->getNickname() + " " + message->params[1];
		std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + HOST + " PRIVMSG " + channel->getName() + " :" + message->params[1] + "\r\n";
		// std::cout << "privmsg: " << msg << std::endl;
		channel->Broadcast(client, msg);
		std::cout << msg << std::endl;
	}
	else{
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			if (it->second->getNickname() == chan_user)
			{
				std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + HOST + " PRIVMSG " + chan_user + " :" + message->params[1] + "\r\n";
				std::cout << "privmsg: " << msg << std::endl;
				send(it->second->getSocket(), msg.c_str(), msg.length(), MSG_DONTWAIT);
				return;
			}
		throw (std::runtime_error(":Privmsg : Client doesnt exist\r\n"));
	}
}

void	Server::nick(Client *client, IRCMessage *message)
{
	if (!message || message->params.empty() || message->params[0].empty())
		throw (std::runtime_error(":server 431 * :No nickname given\r\n"));
	std::string	nickname = message->params[0];
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second->getNickname() == nickname)
			throw (std::runtime_error(":server 433 * " + nickname + " :Nickname is already in use\r\n"));
	if (!client->getNickname().empty())
		this->broadcast(client, ":" + client->getNickname() + " NICK " + nickname + "\r\n");
	client->setNickname(nickname);
}

void	Server::user(Client *client, IRCMessage *msg)
{
	if (msg->params.size() < 4)
		throw (std::runtime_error(":server 461 USER :Not enough parameters\r\n"));
	if (!client->getUsername().empty())
		throw (std::runtime_error(":server 462 :You may not reregister\r\n"));
	client->setUsername(msg->params[0]);
	client->setRealname(msg->params[3]);

	if (!client->getNickname().empty() && !client->getRealname().empty() && client->getPassword() == this->_password)
	{
		std::string	msgWelcome = ":server 001 " + client->getNickname() + " :Welcome to the IRC network\r\n";
		msgWelcome += ":server 002 " + client->getNickname() + " :Your host is ircserv\r\n";
		msgWelcome += ":server 003 " + client->getNickname() + " :This server was created today\r\n";
		msgWelcome += ":server 004 " + client->getNickname() + " ircserv 1.0 o o\r\n";
		send(client->getSocket(), msgWelcome.c_str(), msgWelcome.length(), MSG_DONTWAIT);
		client->setStatus(true);
	}
}

void	Server::pass(Client *client, IRCMessage *msg)
{
	if (msg->params.empty())
		throw (std::runtime_error(":server 461 PASS :Not enough parameters\r\n"));
	if (msg->params[0] != this->_password)
		throw(std::runtime_error("464 Password incorrect\r\n"));
	client->setPassword(msg->params[0]);
}
