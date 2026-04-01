/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:43:21 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/01 15:53:53 by yel-mens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/******************** CONSTRUCTORS ********************/

Server::Server(void)	:	_port(6667), _password("miaou") {}
Server::Server(unsigned short port, std::string password)	:	_port(port), _password(password) {}

/********************* DESTRUCTOR ********************/

Server::~Server(void) {}