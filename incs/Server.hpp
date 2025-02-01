/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:37 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/01 12:48:20 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "utils.hpp"
#include "Command_handler.hpp"

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
    	Command_handler _command_handler;

		void createSocket();
		void acceptClient();
		void handleClient(size_t index);
};


#endif