/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:34 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/05 20:01:51 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/utils.hpp"
#include "../incs/Server.hpp"

int main(int argc, char **argv) {
	if (argc != 3)
		throw std::runtime_error("Usage: ./ircserv <port> <password>");
	Server server(argv[1], argv[2]);
	try {
		signal(SIGINT, Server::signalHandler); // handle (ctrl-c)
		signal(SIGQUIT, Server::signalHandler); // handle (ctrl-\)
		server.runServer();
	} catch (const std::exception &e) {
		server.closeSockets();
		std::cerr << RED << e.what() << RESET << std::endl;
		return 1;
	}
}