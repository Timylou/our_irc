/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:04:41 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/21 17:51:49 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

bool	joinMessage(std::string &buffer, std::string &message)
{
	size_t	pos = buffer.find("\r\n");

	if (pos == std::string::npos)
		return (false);
	message = buffer.substr(0, pos);
	buffer.erase(0, pos + 2);

	return (true);
}

IRCMessage	*IRCparsing(std::string line)
{
	IRCMessage	*msg = new IRCMessage;
	size_t		pos = 0;
	std::string	trailing;

	// prefix
	if (!line.empty() && line[0] == ':')
	{
		size_t end = line.find(' ');
		if (end != std::string::npos)
		{
			msg->prefix = line.substr(1, end - 1);
			line = line.substr(end + 1);
		}
	}

	// trailing
	pos = line.find(" :");
	if (pos != std::string::npos)
	{
		trailing = line.substr(pos + 2);
		line = line.substr(0, pos);
	}

	// command + params
	std::istringstream	iss(line);
	iss >> msg->command;

	std::string	param;
	while (iss >> param)
		msg->params.push_back(param);

	// add trailing
	if (!trailing.empty())
		msg->params.push_back(trailing);

	return msg;
}
