/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:31 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/03 22:21:00 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/Server.hpp"
#include "CommandHandler.hpp"

Server::Server (const std::string &port, const std::string &password) : _port(0), _password(""), _command_handler(*this) {
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
	// Load Message Of The Day from file
    loadMotd("config/motd.txt");
	if (_motd.empty())
		throw std::runtime_error("Error: MOTD file is empty."); // ERR_NOMOTD (422)
}

Server::~Server() {
	// close server socket
	close(_server_fd);
	// Close all client sockets
    for (size_t i = 1; i < _fd_list.size(); ++i) {
        close(_fd_list[i].fd);
    }
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

	// set the socket to non-blocking mode
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error("Error: failed to set socket to non-blocking mode.");
	}
	
	// bind the socket to the IP address and port
	if (bind(_server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		throw std::runtime_error("Error: Bind failed.");
	}
	
	// listen for incoming connections
	if (listen(_server_fd, MAX_CONNECTIONS) < 0) {
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
	while (true) {
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
	// add client socket to the list of file descriptors
	_fd_list.push_back(client_poll);

	logMessage("INFO", "New client connected");
	// sendMotd(new_client);
}

void	Server::handleClient(size_t index) {
	Client &client = _clients[index - 1];
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	
	int bytes_read = recv(client.getFd(), buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
	if (bytes_read <= 0) {
		logMessage("ERROR", "Client disconnected");
		close(client.getFd());
		_fd_list.erase(_fd_list.begin() + index);
		_clients.erase(_clients.begin() + index - 1);
		return ;
	} 
	buffer[bytes_read] = '\0';
	// Accumulate message
    client.buffer += buffer;
    
    // Process complete messages
    size_t pos;
    if ((pos = client.buffer.find('\n')) != std::string::npos) {
        std::string message = client.buffer.substr(0, pos);
        client.buffer.erase(0, pos + 1);
        
        logMessage("RECEIVED", message.c_str());
        
        // Handle command
        _command_handler.handleCommand(_clients[index - 1], message);
    }
	
}

void	Server::sendMotd(Client &client) {
	if (_motd.empty()) {
		std::string response = ":wrtm 422 " + client.getNickname() + " :MOTD is missing\r\n";
        send(client.getFd(), response.c_str(), response.length(), 0);
        return;
	}
	
	std::string serverName = "wrtm";
	// start the MOTD
	std::string response = ":" + serverName + " 375 " + client.getNickname() + " :* " + serverName + " Message of the Day -\r\n";
	send(client.getFd(), response.c_str(), response.length(), 0);
	
	std::stringstream ss(_motd);
	std::string line;
	// send each line of the MOTD
	while (std::getline(ss, line, '\n')) {
		response = ":" + serverName + " 372 " + client.getNickname() + " :* " + line + "\r\n";
		send(client.getFd(), response.c_str(), response.length(), 0);
	}
	// end the MOTD
	response = ":" + serverName + " 376 " + client.getNickname() + " :* End of /MOTD command.\r\n";
	send(client.getFd(), response.c_str(), response.length(), 0);
}

void	Server::loadMotd(const std::string &filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		logMessage("ERROR", "Failed to open MOTD file");
		return;
	}
	
	std::stringstream buffer;
	buffer << file.rdbuf(); // read entire file into buffer
	_motd = buffer.str();
	file.close();
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
