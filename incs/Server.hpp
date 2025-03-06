/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:37 by arepsa            #+#    #+#             */
/*   Updated: 2025/03/06 22:42:43 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "utils.hpp"
#include "Replies.hpp"

#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_ntop
#include <poll.h> // poll
#include <fcntl.h> // fcntl


#define MAX_CONNECTIONS 420
#define USR_MAX_CHANNELS 5
#define BUFFER_SIZE 1024
#define SERVER_NAME "42irc.wrtm.org"

class Client;
class Channel;
class CommandHandler;
class Replies; 

class Server {
	public:
		Server(const int &port, const std::string &password);
		~Server();

		//getters
		int			getPort() const;
		std::string	getPassword() const;
		std::string	getName() const;
		std::map<int, Client>&			getClients();
		std::map<std::string, Channel>&	getChannels();

		//main server functions
		void			runServer(); //start server loop
		void			shutdownServer();
		static void		signalHandler(int signal);
        bool			clientExists(const std::string &nick, Client &client_found);

		//channel functions
        void			addChannel(Client &client, std::string &name, std::string &channel_password);
		int 			checkChannelType(std::string &channel_name);
		bool			channelExists(const std::string &channel_name, Channel& channel_found);
		Channel*		getChannelByName(const std::string &channel_key);

		Replies*		_replies;

	private:
		std::string		_name;
		int				_port;
		std::string		_password;
		int				_server_fd;
		static bool		_running;
		std::map<int, pollfd>			_fd_map;
		std::map<int, Client>			_clients;
		std::map<std::string, Channel>	_channels;
		
    	CommandHandler*	_command_handler;

		//server functions
		void createSocket();
		void acceptClient();
		void handleClient(int client_fd);

		//channel functions
		bool validChannelName(Client &client, std::string &channel_key);
		bool checkUserChannelLimit(Client &client, std::string &channel_key);
		void joinExistingChannel(Client &client, Channel &channel, std::string &channel_key, std::string &channel_password);
		void createNewChannel(Client &client, std::string &channel_key);
};

#endif

