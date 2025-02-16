/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnections.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:57:15 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/16 19:57:17 by uviana-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CommandHandler.hpp"

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
	
	// get client's IP address (host)
	char host[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client_addr.sin_addr, host, INET_ADDRSTRLEN);
	
	//add client to the list of clients
	//_clients[client_fd] = Client(client_fd);
	_clients.insert(std::make_pair(client_fd, Client(client_fd, host)));
	
	// initialize pollfd struct for client socket, POLLIN: there is data to read
	pollfd client_poll = {client_fd, POLLIN, 0};
	if (client_poll.fd < 0) {
		logMessage("ERROR", "Failed to create pollfd struct for client socket");
		close(client_fd);
		return;
	}
	// add client socket to the list of file descriptors
	_fd_map[client_fd] = client_poll;
	logMessage("INFO", "New client connected");
}

void	Server::handleClient(int client_fd) {
	if (_clients.find(client_fd) == _clients.end()){
		logMessage("ERROR", "Client not found in the list of clients");
		return;
	}
	
	Client &client = _clients[client_fd];

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	
	int bytes_read = recv(client.getFd(), buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
	if (bytes_read <= 0) {
		if (bytes_read == 0)
			logMessage("INFO", "Client disconnected");
		else
			logMessage("ERROR", "Failed to receive data from client");
		close(client.getFd());
		_fd_map.erase(client_fd);
		_clients.erase(client_fd);
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
        
        logMessage("RECEIVED >", message.c_str());
        
        // Handle command
		_command_handler->handleCommand(client, message);
    }
}

std::map<int, Client>&Server::getClients() {
	return _clients;
}

bool Server::clientExists(const std::string &nick, Client& client_found)
{
	if (this->_clients.empty())
		return false;
	std::map<int, Client>::iterator it;
	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (it->second.getNickname() == nick)
		{
			client_found = it->second;
			return true;
		}
	}
	return false;
}

