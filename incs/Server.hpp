/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:37 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/04 21:21:00 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "utils.hpp"
#include "CommandHandler.hpp"

#include <sys/socket.h> // socket, bind, listen, accept, send
#include <netinet/in.h> // sockaddr_in
#include <poll.h> // poll
#include <fcntl.h> // fcntl
#include <vector> // std::vector
#include <map> // std::map

#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024

class Server {
	public:
		Server(const std::string &port, const std::string &password);
		~Server();

		int getPort() const;
		std::string getPassword() const;

		void runServer(); //start server loop
		std::vector<Client> getClients() const;
	private:
		int _port;
		std::string _password;
		int _server_fd;
		std::vector<pollfd> _fd_list;
		std::vector<Client> _clients;
    	CommandHandler _command_handler;

		void createSocket();
		void acceptClient();
		void handleClient(size_t index);
};


#endif