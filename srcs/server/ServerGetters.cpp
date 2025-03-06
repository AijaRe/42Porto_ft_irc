/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:57:39 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/06 22:34:55 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int Server::getPort() const {
	return _port;
}

std::string Server::getPassword() const {
	return _password;
}

std::string Server::getName() const{
	return _name;
}

std::map<int, Client>&	Server::getClients() 
{
	return _clients;
}

std::map<std::string, Channel>&Server::getChannels()
{
	return this->_channels;
}
