/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:06:22 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/22 12:57:05 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

/******************** CONSTRUCTOR ********************/

Channel::Channel(const std::string &name)	:	_name(name) {}

/********************* DESTRUCTOR ********************/

Channel::~Channel(void) {}

void	Channel::Broadcast(Client *client, const std::string &message)
{
	for (std::map<Client *, bool>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		send(it->first->getSocket(), message.c_str(), message.length(), MSG_DONTWAIT);
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
