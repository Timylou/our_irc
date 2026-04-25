/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 11:06:30 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/25 18:34:33 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Server;
class Client;
class Channel;

# include <sstream>			// std::istringstream
# include <string>			// std::string
# include <iostream>		// std::cout, std::cerr
# include <algorithm>		// std::find
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

# define CAP_ANS ":server CAP * LS :\r\n"
# define UNREG ":server 451 * :You have not registered\r\n"
# define NOT_FOUND "command not found\r\n"
# define HOST "127.0.0.1"

struct IRCMessage
{
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
};

bool	joinMessage(std::string &buffer, std::string &message);
IRCMessage	*IRCparsing(std::string message);

void	handlePRIVMSG(Server &server, Client *client, IRCMessage *message);
void	handleJOIN(Server &server, Client *client, IRCMessage *message);
void	handleNICK(Server &server, Client *client, IRCMessage *message);
void	handleUSER(Server &server, Client *client, IRCMessage *message);
void	handlePASS(Server &server, Client *client, IRCMessage *message);
