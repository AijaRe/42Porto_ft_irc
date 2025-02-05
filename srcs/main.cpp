/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:34 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/04 16:06:36 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "Server.hpp"

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
