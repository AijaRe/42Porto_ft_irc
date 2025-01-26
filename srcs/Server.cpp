/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:31 by arepsa            #+#    #+#             */
/*   Updated: 2025/01/26 20:58:44 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/Server.hpp"

Server::Server (const std::string &port, const std::string &password) {
	// validate and save port number
	char *end;
	long tmp_port = std::strtol(port.c_str(), &end, 10);
	if (*end != '\0' || tmp_port < 1 || tmp_port > 65535)
		throw std::runtime_error("Error: Invalid port number.");
	_port = static_cast<int>(tmp_port);

	// save password
	_password = password;
	if (_password.empty())
		throw std::runtime_error("Error: Password cannot be empty.");

	createSocket();

}

Server::~Server() {
	// close server socket
	close(_server_fd);
}

void	Server::createSocket() {
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
		throw std::runtime_error("Error: Socket creation failed.");

	struct sockaddr_in serv_addr;
	
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; // IPv4 family
	serv_addr.sin_addr.s_addr = INADDR_ANY; // Any IP address
	serv_addr.sin_port = htons(_port); // convert port to network byte order

	// reuse port after server restart
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		throw std::runtime_error("Error: setsockopt failed.");
	}

	// place for fcntl???
	
	// bind the socket to the IP address and port
	if (bind(_server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		throw std::runtime_error("Error: Bind failed.");
	}
	
	// listen for incoming connections
	if (listen(_server_fd, MAX_CONNECTIONS) < 0) {
		throw std::runtime_error("Error: Listen failed.");
	}

	logMessage("INFO", "Server listening on port " + std::to_string(_port));
	
}

int Server::getPort() const {
	return _port;
}

std::string Server::getPassword() const {
	return _password;
}
