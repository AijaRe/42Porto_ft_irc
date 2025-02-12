/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:31 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/12 19:41:19 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CommandHandler.hpp"
#include "Channel.hpp"
#include "Client.hpp"

bool Server::_running = true;

Server::Server (const std::string &port, const std::string &password) : _name("42_IRC"), _port(0), _password(""), _server_fd(-1) /*_command_handler(*this)*/ {
	// validate and save port number
	char *end;
	long tmp_port = std::strtol(port.c_str(), &end, 10);
	if (*end != '\0' || tmp_port < 1 || tmp_port > 65535)
		throw std::runtime_error("Error: Invalid port number.");
	_port = static_cast<int>(tmp_port);
	
	_command_handler = new CommandHandler(*this);
	// save password
	_password = password;
	if (_password.empty())
		throw std::runtime_error("Error: Password cannot be empty.");

	createSocket();
}

Server::~Server() {
	delete _command_handler; 
	// shutdownServer();
}

void	Server::signalHandler(int signal) {
	(void)signal;
	_running = false;
}

void	Server::shutdownServer() {
	/* if (!_running)
		return; // server already shut down */
	_running = false;
	logMessage("INFO", "Shutting down server...");
	// close server socket
	if (_server_fd >= 0)
		close(_server_fd);
	// Close all client sockets
	for (std::map<int, pollfd>::iterator it = _fd_map.begin(); it != _fd_map.end(); ++it) {
		if (it->first >= 0)
			close(it->first);
	}
	// Reset the list of file descriptors and clients
	_fd_map.clear();
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
	_fd_map[_server_fd] = server_poll;
}

void	Server::runServer() {
	while (this->_running) {
		// use poll to wait for events on the sockets
		/*Another thing interesting in the approach is that the poll function will 
		verify for new connection on the first _fd_list position (since its the _server_fd position)
		and in the following ones (client connected) it will monitor for data to read.*/
		// Convert the map to a vector of pollfd for the poll function
        std::vector<pollfd> poll_fds;
        for (std::map<int, pollfd>::const_iterator it = _fd_map.begin(); it != _fd_map.end(); ++it) {
            poll_fds.push_back(it->second);
        }
		int activity = poll(poll_fds.data(), poll_fds.size(), -1); // -1: wait indefinitely
		
		if (activity < 0) {
			 if (_running) {
                throw std::runtime_error("Error: Poll failed.");
            } else {
                break; // Exit the loop if _running is false
            }
		}

		// check if the server socket has an event (new client connection)
		if (poll_fds[0].revents & POLLIN) {
			acceptClient();
		}

		// check existing clients for events (incoming messages)
		for (size_t i = 1; i < poll_fds.size(); ++i) {
			if (poll_fds[i].revents & POLLIN) {
				handleClient(poll_fds[i].fd);
			}
		}
	}
	//Clean the code
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

int Server::getPort() const {
	return _port;
}

std::string Server::getPassword() const {
	return _password;
}

std::map<int, Client>&Server::getClients() {
	return _clients;
}

std::string Server::getName() const{
	return _name;
}

int		Server::checkChannelType(std::string &channel_name)
{
	char type = channel_name[0];
	switch (type)
    {
        case '#':
            return CH_STANDARD;
        case '&':
            return CH_LOCAL;
        case '+':
            return CH_NO_EXT;
        case '!':
            return CH_SAFE;
        default:
            return USER;
    }
}

//Verifica se o channel está no map e caso positivo retorna true.
bool Server::getChannel(const std::string &channel_key, Channel& channel_found)
{
	std::map<std::string, Channel>::iterator it;
	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if (it->first == channel_key)
		{
			channel_found = it->second;
			return true;
		}
	};
	return false;
}

void	Server::addChannel(const Client &client, std::string &channel_key) //Channel Key = [Key + Channel name], for example #chan1 or !eita or &alochannel
{
	/*Needs to add a verifications to the channel name, before even joining the channel, here are some errors:
    Error Code	HexChat Error Message	            Description
    461	        Not enough parameters	            The JOIN command requires a channel name, but none was provided.
    403	        No such channel	                    The channel name is invalid (e.g., missing # or contains illegal characters).
    405	        You have joined too many channels	The user has exceeded the server's limit for the number of joined channels.
    474     	Cannot join channel (+b)	        The user is banned from the channel.
    473	        Cannot join channel (+i)	        The channel is invite-only, and the user has not been invited.
    475	        Cannot join channel (+k)	        The channel requires a password, and the user provided none or an incorrect one.
    471	        Cannot join channel (+l)	        The channel is full and cannot accept more users.
    476	        Bad channel mask	                The channel name is invalid (e.g., contains illegal characters).
    479	        Erroneous channel name	            The channel name is invalid (e.g., contains spaces or disallowed characters).
    437	        Channel temporarily unavailable 	The channel is temporarily unavailable (e.g., being registered or reserved).
    484	        Your connection is restricted	    The user's connection is restricted and cannot join the channel.
    480	        Joining channels too fast	        The user is joining channels too quickly and is being throttled by the server.
    443	        is already on channel	            The user is already on the specified channel.
    442	        You're not on that channel	        The user is not on the specified channel.
    407     	Too many targets	                The user is trying to join too many channels at once.*/
	int channel_type = this->checkChannelType(channel_key);
	if (channel_type == 0)
	{
		std::cout << "[TO DO] ERROR: Invalid Channel name." << std::endl;
		return;
	}
	else
	{
		Channel channel_found;
		if (this->getChannel(channel_key, channel_found)) //if True, a channel with this channel_key was found;
		{
			std::cout << "[TO DO] Check user permissions to access this channel before JOIN." << std::endl;
			channel_found.addClient(client);
		}
		else
		{
			std::cout << "[TO DO] Check user permissions to create this type of channel." << std::endl;
			Channel new_channel(client, channel_key, channel_type);
			std::pair<std::map<std::string, Channel>::iterator, bool> ret;
			ret = this->_channels.insert(std::make_pair(channel_key, new_channel));
			if (ret.second == false)
				sendError(client.getFd(), "There is already a channel called " + channel_key + ".\n");
		}
	}
}
	

	
