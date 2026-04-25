/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 14:09:51 by julifern          #+#    #+#             */
/*   Updated: 2026/04/25 17:27:07 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void	handleUSER(Server &server, Client *client, IRCMessage *msg)
{
	if (msg->params.size() < 4)
		throw (std::runtime_error(":server 461 USER :Not enough parameters\r\n"));
	if (!client->getUsername().empty())
		throw (std::runtime_error(":server 462 :You may not reregister\r\n"));
	client->setUsername(msg->params[0]);
	client->setRealname(msg->params[3]);

	if (!client->getNickname().empty() && !client->getRealname().empty() && client->getPassword() == server.getPassword())
	{
		std::string	msgWelcome = ":server 001 " + client->getNickname() + " :Welcome to the IRC network\r\n";
		msgWelcome += ":server 002 " + client->getNickname() + " :Your host is ircserv\r\n";
		msgWelcome += ":server 003 " + client->getNickname() + " :This server was created today\r\n";
		msgWelcome += ":server 004 " + client->getNickname() + " ircserv 1.0 o o\r\n";
		send(client->getSocket(), msgWelcome.c_str(), msgWelcome.length(), MSG_DONTWAIT);
		client->setStatus(true);
	}
}
