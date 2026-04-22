/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:39:16 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/22 13:03:31 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# define MAX_CLIENTS 100
# define BUFFER_SIZE 1024

# include "irc.hpp"

class Client;
class Channel;

class Server
{
	private:
		int									_listenSocket;	// socket to listen clients
		unsigned short						_port;			// port's server
		std::string							_password;		// server's password
		std::map<int, Client *>				_clients;		// map of all clients with user info. int = socket
		std::vector<pollfd>					_pfd;			// array of all clients pollfd struct
		std::map<std::string, Channel *>	_channels;		// array of channels

	public:
		Server(std::string port, std::string password);
		~Server(void);

		void	run(void);

		int				getSocket(void) const;
		unsigned short	getPort(void) const;
		std::string		getPassword(void) const;

		void	addClient(int socket);
		void	removeClient(Client *client, int numClient);

		int		readMessage(Client *client);
		void	broadcast(Client *client, std::string message);
		std::vector<Client *>	getListenningClients(Client *client);

		bool	findChannel(const std::string &channelName);
		void	addChannel(const std::string &channelName);

		void	doCmd(Client *client, std::string line);
		void	join(Client *client, IRCMessage *message);
		void	privmsg(Client *client, IRCMessage *message);
		void	nick(Client *client, IRCMessage *message);
		void	user(Client *client, IRCMessage *message);
		void	pass(Client *client, IRCMessage *message);
};
