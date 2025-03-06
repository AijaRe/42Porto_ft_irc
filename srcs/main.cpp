/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:34 by arepsa            #+#    #+#             */
/*   Updated: 2025/03/06 22:39:58 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "Server.hpp"

int main(int argc, char **argv) 
{
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	// Validate port range and empty password before creating Server object
	char *end;
	long port_arg = std::strtol(argv[1], &end, 10);
	if (*end != '\0' || port_arg < 1024 || port_arg > 65535) {
		std::cerr << "Error: Invalid or restricted port number." << std::endl;
		return 1;
	}
	std::string password = argv[2];
	if (password.empty()) {
		std::cerr << "Error: Password cannot be empty." << std::endl;
		return 1;
	}
	Server server(static_cast<int>(port_arg), password);
	try {
		signal(SIGINT, Server::signalHandler); // handle (ctrl-c)
		signal(SIGQUIT, Server::signalHandler); // handle (ctrl-\)
		//gnal(SIGTSTP, Server::signalHandler); // handle (ctrl-z)
		server.runServer();
	} catch (const std::exception &e) {
		std::cerr << RED << e.what() << RESET << std::endl;
		server.shutdownServer();
		return 1;
	}
	server.shutdownServer();
	return 0;
}
