/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:39:16 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/01 15:47:20 by yel-mens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <vector>

class Server
{
	private:
		unsigned short		_port;
		const std::string	_password;
	
	public:
		Server(void);
		Server(unsigned short port, std::string password);
		~Server(void);
};

#endif