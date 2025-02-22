/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:37 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/22 15:58:19 by arepsa           ###   ########.fr       */
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

#define MAX_CONNECTIONS 100
#define USR_MAX_CHANNELS 5
#define BUFFER_SIZE 1024
#define SERVER_NAME "42_IRC"

class Client;
class Channel;
class CommandHandler;
class Replies; 

class Server {
	public:
		Server(const int &port, const std::string &password);
		~Server();

		int getPort() const;
		std::string getPassword() const;
		std::string getName() const;

		void runServer(); //start server loop
		static void signalHandler(int signal);
		void shutdownServer();
		std::map<int, Client>&getClients();
		bool clientExists(const std::string &nick, Client& client_found);

		
		void addChannel(Client &client, std::string &name, std::string &channel_password);
		int  checkChannelType(std::string &channel_name);
		std::map<std::string, Channel> &getChannels();
		bool channelExists(const std::string &channel_name, Channel& channel_found);

		Replies *_replies;

	private:
		std::string _name;
		int _port;
		std::string _password;
		int _server_fd;
		static bool _running;
		std::map<int, pollfd> _fd_map;
		std::map<int, Client> _clients;
		std::map<std::string, Channel> _channels;
		
    	CommandHandler* _command_handler;

		void createSocket();
		void acceptClient();
		void handleClient(int client_fd);
};

#endif

