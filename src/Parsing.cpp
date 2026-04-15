/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:04:41 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/13 18:29:35 by yel-mens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

IRCMessage	*IRCparsing(std::string line)
{
	IRCMessage	*msg = new IRCMessage;
	size_t		pos;
	std::string	trailing;

	// 🔹 trailing
	pos = line.find(" :");
	if (pos != std::string::npos)
	{
		trailing = line.substr(pos + 2);
		line = line.substr(0, pos);
	}

	// 🔹 command + params
	std::istringstream	iss(line);
	iss >> msg->command;

	std::string	param;
	while (iss >> param)
		msg->params.push_back(param);

	// 🔹 add trailing
	if (!trailing.empty())
		msg->params.push_back(trailing);

	return msg;
}