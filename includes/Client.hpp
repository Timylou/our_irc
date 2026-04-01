/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:47:35 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/01 15:59:48 by yel-mens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <iostream>

class Client
{
	private:
		int			_socket;	// the fd of the socket
		std::string	_name;		// the client's name
	
		Client(void);			// We can't use the void constructor

	public:
		Client(int socket, std::string name);
		~Client(void);
};

#endif