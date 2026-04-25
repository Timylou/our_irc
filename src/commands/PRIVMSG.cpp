/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 13:15:31 by julifern          #+#    #+#             */
/*   Updated: 2026/04/25 19:35:25 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void	handlePRIVMSG(Server &server, Client *client, IRCMessage *message)
{
	std::cout << "here" << std::endl;
	if (!message || message->params.empty() || message->params[0].empty())
		throw (std::runtime_error(":server 461 PRIVMSG :Not enough parameters\n"));

	std::string target = message->params[0];
	std::string text;
	for (size_t i = 1; i < message->params.size(); i++)
    {
        if (i > 1)
            text += " ";
        text += message->params[i];
    }
	std::string fullMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + HOST + " PRIVMSG " + target + " :" + message->params[1] + "\r\n";

	if (target[0] == '#') {
		if (!server.findChannel(target))
			throw (std::runtime_error(":server 403 " + target + " :No such channel\r\n"));
		Channel	*channel = server.getChannel(target);
		if (!channel->findClient(client))
			throw (std::runtime_error(":server 442 " + target + " :You're not on that channel\r\n"));
		channel->broadcast(fullMsg);
	}

	else {
		Client *destination = server.getClient(target);
		if (!destination)
			throw (std::runtime_error(":server 401 " + target + " :No such nick\r\n"));
		sendToClient(destination, fullMsg);
	}
}
