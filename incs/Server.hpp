/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:37 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/12 20:43:30 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "utils.hpp"
#include "Replies.hpp"

#include <sys/socket.h> // socket, bind, listen, accept, send
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_ntop
#include <poll.h> // poll
#include <fcntl.h> // fcntl
#include <vector> // std::vector
#include <map> // std::map

#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024

class Client;
class Channel;
class CommandHandler;

class Server {
	public:
		Server(const std::string &port, const std::string &password);
		~Server();

		int getPort() const;
		std::string getPassword() const;
		std::string getName() const;

		void runServer(); //start server loop
		static void signalHandler(int signal);
		void shutdownServer();
		std::map<int, Client>&getClients();

		void addChannel(const Client &client, std::string &name);
		int  checkChannelType(std::string &channel_name);
		bool getChannel(const std::string &channel_name, Channel& channel_found);



		//TEMP PUBLIC TO DO TESTS
		std::map<std::string, Channel> _channels;
	private:
		std::string _name;
		int _port;
		std::string _password;
		int _server_fd;
		static bool _running;
		std::map<int, pollfd> _fd_map;
		std::map<int, Client> _clients;
		
    	CommandHandler* _command_handler;

		void createSocket();
		void acceptClient();
		void handleClient(int client_fd);
};

#endif
