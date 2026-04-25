/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 14:17:52 by julifern          #+#    #+#             */
/*   Updated: 2026/04/25 17:27:03 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void	handlePASS(Server &server, Client *client, IRCMessage *msg)
{
	if (msg->params.empty())
		throw (std::runtime_error(":server 461 PASS :Not enough parameters\r\n"));
	if (msg->params[0] != server.getPassword())
		throw(std::runtime_error("464 Password incorrect\r\n"));
	client->setPassword(msg->params[0]);
}
