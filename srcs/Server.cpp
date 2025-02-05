/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:31 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/05 20:36:53 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/Server.hpp"
#include "CommandHandler.hpp"

bool Server::_running = true;

Server::Server (const std::string &port, const std::string &password) : _port(0), _password(""), _server_fd(-1), _command_handler(*this) {
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
	closeSockets();
}

void	Server::signalHandler(int signal) {
	(void)signal;
	_running = false;
	logMessage("INFO", "Server shutting down");
}

void	Server::closeSockets() {
	// close server socket
	if (_server_fd >= 0)
		close(_server_fd);
	// Close all client sockets
	for (size_t i = 1; i < _fd_list.size(); ++i) {
		if (_fd_list[i].fd >= 0)
			close(_fd_list[i].fd);
	}
	// Reset the list of file descriptors and clients
	_fd_list.clear();
	_clients.clear();
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
		close(_server_fd);
		throw std::runtime_error("Error: setsockopt failed.");
	}

	// set the socket to non-blocking mode
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0) {
		close(_server_fd);
		throw std::runtime_error("Error: failed to set socket to non-blocking mode.");
	}
	
	// bind the socket to the IP address and port
	if (bind(_server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		close(_server_fd);
		throw std::runtime_error("Error: Bind failed.");
	}
	
	// listen for incoming connections
	if (listen(_server_fd, MAX_CONNECTIONS) < 0) {
		close(_server_fd);
		throw std::runtime_error("Error: Listen failed.");
	}
	std::stringstream ss;
	ss << _port;
	logMessage("INFO", "Server listening on port " + ss.str());

	// initialize pollfd struct for server socket, POLLIN: there is data to read
	pollfd server_poll = {_server_fd, POLLIN, 0};
	// add server socket to the list of file descriptors (fd with index 0)
	_fd_list.push_back(server_poll); 
}

void	Server::runServer() {
	while (this->_running) {
		// use poll to wait for events on the sockets
		int activity = poll(_fd_list.data(), _fd_list.size(), -1); // -1: wait indefinitely
		if (activity < 0) {
			logMessage("ERROR", "Poll failed");
			break;
		}

		// check if the server socket has an event (new client connection)
		if (_fd_list[0].revents & POLLIN) {
			acceptClient();
		}

		// check existing clients for events (incoming messages)
		for (size_t i = 1; i < _fd_list.size(); ++i) {
			if (_fd_list[i].revents & POLLIN) {
				handleClient(i);
			}
		}
	}
}

void	Server::acceptClient() {
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(client_addr);
	int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &addrlen);
	if (client_fd < 0) {
		logMessage("ERROR", "Accept failed");
		return;
	}
	// set the client socket to non-blocking mode
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
		logMessage("ERROR", "Failed to set client socket to non-blocking mode");
		close(client_fd);
		return;
	}
	
	//add client to the list of clients
	Client new_client(client_fd);
    _clients.push_back(new_client);
	
	// initialize pollfd struct for client socket, POLLIN: there is data to read
	pollfd client_poll = {client_fd, POLLIN, 0};
	if (client_poll.fd < 0) {
		logMessage("ERROR", "Failed to create pollfd struct for client socket");
		close(client_fd);
		return;
	}
	// add client socket to the list of file descriptors
	_fd_list.push_back(client_poll);

	logMessage("INFO", "New client connected");
}

void	Server::handleClient(size_t index) {
	Client &client = _clients[index - 1];
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	
	int bytes_read = recv(client.getFd(), buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
	if (bytes_read <= 0) {
		if (bytes_read == 0)
			logMessage("INFO", "Client disconnected");
		else
			logMessage("ERROR", "Failed to receive data from client");
		close(cli ent.getFd());
		_fd_list.erase(_fd_list.begin() + index);
		_clients.erase(_clients.begin() + index - 1);
		return ;
	} 
	buffer[bytes_read] = '\0';
	// Accumulate message
    client.buffer += buffer;
    
    // Process complete messages
    size_t pos;
    while ((pos = client.buffer.find('\n')) != std::string::npos) {
        std::string message = client.buffer.substr(0, pos);
        client.buffer.erase(0, pos + 1);
        
        logMessage("RECEIVED", message.c_str());
        
        // Handle command
        _command_handler.handleCommand(_clients[index - 1], message);
    }
}

int Server::getPort() const {
	return _port;
}

std::string Server::getPassword() const {
	return _password;
}

std::vector<Client> Server::getClients() const {
	return _clients;
}