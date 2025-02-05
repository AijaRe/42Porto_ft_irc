/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:37 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/04 16:06:29 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "utils.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "CommandHandler.hpp"

#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024

class Channel;

class Server {
	public:
		Server(const std::string &port, const std::string &password);
		~Server();

		int getPort() const;
		std::string getPassword() const;

		void runServer(); //start server loop
		std::vector<Client> getClients() const;

		void addChannel(const Client &client, const std::string &name);


		std::vector<Channel> _channels; //TEMP PUBLIC TO DO TESTS
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
