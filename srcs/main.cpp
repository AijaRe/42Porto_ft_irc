/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:34 by arepsa            #+#    #+#             */
/*   Updated: 2025/01/29 21:04:48 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

int main(int argc, char **argv) {
	try {
		if (argc != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		Server server(argv[1], argv[2]);
		server.runServer();
	} catch (const std::exception &e) {
		std::cerr << RED << e.what() << RESET << std::endl;
		return 1;
	}
}