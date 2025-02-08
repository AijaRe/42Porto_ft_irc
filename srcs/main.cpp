/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:34 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/08 14:57:34 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {
	if (argc != 3)
		throw std::runtime_error("Usage: ./ircserv <port> <password>");
	Server server(argv[1], argv[2]);
	try {
		signal(SIGINT, Server::signalHandler); // handle (ctrl-c)
		signal(SIGQUIT, Server::signalHandler); // handle (ctrl-\)
		signal(SIGTSTP, Server::signalHandler); // handle (ctrl-z)
		server.runServer();
	} catch (const std::exception &e) {
		std::cerr << RED << e.what() << RESET << std::endl;
		server.shutdownServer();
		return 1;
	}
	server.shutdownServer();
	return 0;
}
