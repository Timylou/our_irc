/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:39:16 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/08 19:25:44 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>		// std::cout, std::cerr
# include <map>				// std::map
# include <string>			// std::string
# include <vector>			// std::vector
# include <stdexcept>		// std::runtime_error
# include <cstring>			// std::memset
# include <cerrno>			// errno
# include <csignal>			// sigaction, SIGINT, SIGTSTP
# include <unistd.h>		// close()
# include <sys/types.h>		// types génériques pour socket
# include <sys/socket.h>	// socket(), bind(), listen(), accept(), setsockopt()
# include <netinet/in.h> 	// sockaddr_in, in_addr, htons(), INADDR_ANY, AF_INET
# include <arpa/inet.h>		// inet_ntoa(), inet_pton(), inet_ntop()
# include <ifaddrs.h>		// getifaddrs()
# include <poll.h>			// poll(), pollfd

# include "Client.hpp"

# define MAX_CLIENTS 100
# define BUFFER_SIZE 1024

class Server
{
	private:
		int						_listenSocket;	// socket to listen clients
		unsigned short			_port;			// port's server
		const std::string		_password;		// server's password
		std::map<int, Client *>	_clients;		// map of all clients with user info
		std::vector<pollfd>		_pfd;			// array of all clients pollfd struct

	public:
		Server(void);
		Server(unsigned short port, std::string password);
		~Server(void);

		void	run(void);

		int				getSocket(void) const;
		unsigned short	getPort(void) const;
		std::string		getPassword(void) const;

		void	addClient(int socket);
		void	removeClient(Client *client, int numClient);

		bool	readMessage(Client *client);
		void	broadcast(Client *client);
};

#endif
