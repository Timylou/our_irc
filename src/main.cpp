/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-mens <yel-mens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:13:10 by yel-mens          #+#    #+#             */
/*   Updated: 2026/04/02 17:28:48 by amairia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

int	main(int argc, char **argv)
{
	try {
		Server	server;
		server.run();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}
