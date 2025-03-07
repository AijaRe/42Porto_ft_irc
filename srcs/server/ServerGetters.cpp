/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerGetters.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:57:39 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/07 21:55:32 by arepsa           ###   ########.fr       */
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

std::time_t Server::getCreatedOn() const
{
	return _created_on;
}
