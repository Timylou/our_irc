/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:00:40 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/01 16:06:04 by yel-mens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include <iostream>
# include <vector>
# include <map>
# include "Client.hpp"

class Channel
{
	private:
		std::string				_name;		// the name of the channel
		std::vector<Client *>	_clients;	// array of the channel's client
		std::string				_topic;		// the topic of the current channel

	public:
		Channel(const std::string &name);
		~Channel(void);
};

#endif