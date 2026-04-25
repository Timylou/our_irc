/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:06:22 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/25 19:22:43 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

/******************** CONSTRUCTOR ********************/

Channel::Channel(const std::string &name)	:	_name(name) {}

/********************* DESTRUCTOR ********************/

Channel::~Channel(void) {}

void	Channel::broadcast(const std::string &message)
{
	for (std::map<Client *, bool>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::cout << "SEND to ptr=" << it->first
          << " nick=" << it->first->getNickname()
          << " fd=" << it->first->getSocket() << std::endl;
		send(it->first->getSocket(), message.c_str(), message.length(), MSG_DONTWAIT);
	}
}

std::string	Channel::getStringClient(void)
{
	std::string clients;
	for (std::map<Client *, bool>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it != _clients.begin())
			clients += ' ';
		clients += it->first->getNickname();
	}
	return (clients);
}
