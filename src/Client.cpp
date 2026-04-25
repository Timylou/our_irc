/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:53:33 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/25 18:36:43 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

void	sendToClient(Client *client, std::string msg)
{
	send(client->getSocket(), msg.c_str(), msg.size(), MSG_DONTWAIT);
}

/********************* DESTRUCTOR *********************/

Client::~Client(void) {}
