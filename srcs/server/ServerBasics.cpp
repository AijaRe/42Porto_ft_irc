/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBasics.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:31 by arepsa            #+#    #+#             */
/*   Updated: 2025/03/07 21:48:33 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CommandHandler.hpp"
#include "Channel.hpp"
#include "Client.hpp"

bool Server::_running = true;

Server::Server (const int &port, const std::string &password) : _name(SERVER_NAME), _port(port), _password(password), _server_fd(-1)
{
	if (_port < 1024 || _port > 65535) {
		std::cerr << "Error: Invalid or restricted port number." << std::endl;
		_running = false;
		return;
	}
	_command_handler = new CommandHandler(*this);
	if (_password.empty()) {
		logMessage("ERROR", "Password cannot be empty.");
		delete _command_handler;
		_running = false;
		return;
	}
	
	try {
		createSocket();
	} catch (const std::exception &e) {
		std::cerr << RED << e.what() << RESET << std::endl;
		_running = false;
		delete _command_handler;
		exit(EXIT_FAILURE);
	}
	//get current timestamp
	_created_on = std::time(0);
}

Server::~Server()
{
	delete _command_handler; 
	// shutdownServer();
}

void	Server::signalHandler(int signal)
{
	(void)signal;
	_running = false;
}

void	Server::shutdownServer()
{
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


void	Server::createSocket()
{
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
		delete _command_handler;
		logMessage("ERROR", "Socket bind failed.");
		exit(EXIT_FAILURE);
	}
	
	// listen for incoming connections
	if (listen(_server_fd, MAX_CONNECTIONS) < 0) {
		close(_server_fd);
		delete _command_handler;
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
		// Poll function will verify for new connection on the first _fd_list position (_server_fd)
		// and in the following ones (client connected) it will monitor for data to read.*/
		// Use poll to wait for events on the sockets
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
}

