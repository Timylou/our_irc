/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:39:16 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/25 18:36:59 by julifern         ###   ########.fr       */
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

		int				getSocket(void) const {return (this->_listenSocket);}
		unsigned short	getPort(void) const {return (this->_port);}
		std::string		getPassword(void) const {return (this->_password);}

		void	addClient(int socket);
		void	removeClient(Client *client, int numClient);

		Client	*getClient(std::string nickname);
		std::map<int, Client *> &getClients(void) { return this->_clients; }

		int		readMessage(Client *client);

		bool	findChannel(const std::string &channelName);
		void	addChannel(const std::string &channelName);

		Channel*	&getChannel(std::string user){
			std::map<std::string, Channel *>::iterator it = _channels.find(user);
			if (it == _channels.end())
				throw std::runtime_error("Channel not found");
			return it->second;
		}
		std::map<std::string, Channel *>	&getChannels(void) { return this->_channels; }

		void	doCmd(Client *client, std::string line);
};
