/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:37 by arepsa            #+#    #+#             */
/*   Updated: 2025/01/31 18:48:35 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "utils.hpp"

#include <sys/socket.h> // socket, bind, listen, accept, send
#include <netinet/in.h> // sockaddr_in
#include <poll.h> // poll
#include <map>
#include "CommandHandler.hpp"
#include "User.hpp"


#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024

class Server {
	public:
		Server(const std::string &port, const std::string &password);
		~Server();

		int getPort() const;
		std::string getPassword() const;

		void runServer(); //start server loop
		void handleClient(size_t index);
		std::map<int, User> _users;
		
	private:
		int _port;
		std::string _password;
		int _server_fd;
		std::vector<pollfd> _fd_list;

		void createSocket();
		void acceptClient();

		CommandHandler _commandHandler;
};


#endif