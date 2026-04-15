/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:06:22 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/13 18:05:49 by yel-mens         ###   ########.fr       */
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
		if (it->first != client)
			send(it->first->getSocket(), message.c_str(), message.length(), MSG_DONTWAIT);
}