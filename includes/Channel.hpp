/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:00:40 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/09 15:35:25 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include <iostream>
# include <vector>
# include <map>
# include "Client.hpp"

typedef enum e_mode
{
	MODE_I,
	MODE_T,
	MODE_K,
	MODE_O,
	MODE_L
}	t_mode;



class Channel
{
	private:
		std::string					_name;		// the name of the channel
		std::string					_topic;		// the topic of the current channel
		std::map<t_mode, bool>		_modes;		// map of channel modes
		std::map<Client *, bool>	_clients;	// map of clients

	public:
		Channel(const std::string &name);
		~Channel(void);
};

#endif
