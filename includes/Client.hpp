/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julifern <julifern@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:47:35 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/07 15:05:50 by julifern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <iostream>
# include <poll.h>			// poll(), pollfd

class Client
{
	private:
		int			_socket = 0;	// the fd of the socket
		std::string	_username = "client";	// client's username
		std::string	_nickname = "";	// hexchat specific
		std::string	_realname = "";	// hexchat specific
		bool		_status = false;	// online status : true for online, false for offline
		std::string	_buffer = "";	// message entry buffer

		Client(void);			// We can't use the void constructor

	public:
		Client(int socket) { this->_socket = socket; }

		void		setStatus(bool newStatus)		{ this->_status = newStatus; }

		std::string	&getBuffer()					{ return (this->_buffer); }
		void		setBuffer(std::string buffer)	{ this->_buffer = buffer; }

		int			getSocket() const				{ return (this->_socket); }

		std::string	getUsername()					{ return (this->_username); }
		std::string	getNickname()					{ return (this->_nickname); }
		std::string	getRealname()					{ return (this->_realname); }

		~Client(void);
};

#endif
