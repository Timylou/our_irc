/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:00:40 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/16 15:15:09 by yel-mens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "irc.hpp"

typedef enum e_mode
{
	MODE_I,
	MODE_T,
	MODE_K,
	MODE_O,
	MODE_L
}	t_mode;

class Client;

class Channel
{
	private:
		std::string					_name;		// the name of the channel
		std::string					_topic;		// the topic of the current channel
		std::string					_password;	// the channel's password
		std::map<t_mode, bool>		_modes;		// map of channel modes
		std::map<Client *, bool>	_clients;	// map of clients. bool = operator status

	public:
		Channel(const std::string &name);
		~Channel(void);

		bool						findClient(Client *client) { return (_clients.find(client) != _clients.end()); }
		std::map<Client *, bool>	getClients(void) { return this->_clients; }
		std::string					getStringClient(void);
		void						addClient(Client *client) {_clients[client] = false;}
		void						removeClient(Client *client) {_clients.erase(client);}
		void						promoteClient(Client *client) {if (findClient(client)) _clients[client] = true;}
		void						demoteClient(Client *client) {if (findClient(client)) _clients[client] = false;}

		std::string					getPassword(void) {return (_password);}
		bool						getMode(t_mode mode) { return (_modes[mode]); }

		std::string					getTopic(void) {return _topic;}
		void						Broadcast(Client *client, const std::string &message);
};
