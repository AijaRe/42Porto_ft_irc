/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:37 by arepsa            #+#    #+#             */
/*   Updated: 2025/01/26 21:04:55 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP


#include "utils.hpp"

#include <sys/socket.h> // socket, bind, listen, accept, send
#include <netinet/in.h> // sockaddr_in
#include <poll.h> // poll

#define MAX_CONNECTIONS 5

class Server {
	public:
		Server(const std::string &port, const std::string &password);
		~Server();

		int getPort() const;
		std::string getPassword() const;
		
	private:
		int _port;
		std::string _password;
		int _server_fd;

		void createSocket();
};


#endif