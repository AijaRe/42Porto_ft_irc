/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClients.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:57:39 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/16 19:57:41 by uviana-a         ###   ########.fr       */
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

