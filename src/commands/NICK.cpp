/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 14:07:32 by julifern          #+#    #+#             */
/*   Updated: 2026/04/25 18:25:05 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void	handleNICK(Server &server, Client *client, IRCMessage *message)
{
	if (!message || message->params.empty() || message->params[0].empty())
		throw (std::runtime_error(":server 431 * :No nickname given\r\n"));

	std::string	newNickname = message->params[0];
	Client *existing = server.getClient(newNickname);
	if (existing && existing != client)
		throw (std::runtime_error(":server 433 * " + newNickname + " :Nickname is already in use\r\n"));

	std::string oldNickname = client->getNickname();

	std::string broadcastMessage = ":" + oldNickname + "!" + client->getUsername() + "@" + HOST + " NICK :" + newNickname + "\r\n";
	for (std::map<std::string, Channel *>::iterator it = server.getChannels().begin(); it != server.getChannels().end(); ++it) {
		if (it->second->findClient(client))
			it->second->broadcast(broadcastMessage);
	}
	client->setNickname(newNickname);
}


