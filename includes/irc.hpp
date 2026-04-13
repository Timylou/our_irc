/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 11:06:30 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/10 16:22:52 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"

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
