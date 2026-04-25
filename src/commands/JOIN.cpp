/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 13:52:46 by julifern          #+#    #+#             */
/*   Updated: 2026/04/25 19:33:59 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void	handleJOIN(Server &server, Client *client, IRCMessage *message)
{
	if (!message || message->params.empty() || message->params[0].empty())
		throw (std::runtime_error("Join command needs parameter\n"));

	std::string	channelName = message->params[0];

	if (channelName[0] != '#')
		throw (std::runtime_error("Channel name always begind with #\n"));

	Channel *channel;

	if (!server.findChannel(channelName)) {
		server.addChannel(channelName);
		channel = server.getChannel(channelName);
		channel->addClient(client);
		channel->promoteClient(client);
	}
	else {
		channel = server.getChannel(channelName);
		if (channel->findClient(client))
			return;
		if (channel->getMode(MODE_K))
		{
			if (message->params.size() < 2 || message->params[1] != server.getChannel(channelName)->getPassword())
				throw (std::runtime_error(":server 475 " + channelName + " :Cannot join channel (+k)\r\n"));
		}
		if (channel->getMode(MODE_I))
			throw (std::runtime_error(":server 473 " + channelName + " :Cannot join channel (+i)\r\n"));
		channel->addClient(client);
	}
	std::string fullMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + HOST + " JOIN " + channelName + "\r\n";
	channel->broadcast(fullMsg);

	std::string reply;
	if (!channel->getTopic().empty())
		reply = ":server 332 " + client->getNickname() + " " + channelName + " :topic\r\n";
	else
		reply = ":server 331 " + client->getNickname() + " " + channelName + " :No topic is set\r\n";
	send(client->getSocket(), reply.c_str(), reply.length(), MSG_DONTWAIT);

	reply = ":server 353 " + client->getNickname() + " = " + channelName + " :" + server.getChannel(channelName)->getStringClient() + "\r\n";
	send(client->getSocket(), reply.c_str(), reply.length(), MSG_DONTWAIT);
	reply = ":server 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
	send(client->getSocket(), reply.c_str(), reply.length(), MSG_DONTWAIT);
}
