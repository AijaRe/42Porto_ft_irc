/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:07:41 by yfontene          #+#    #+#             */
/*   Updated: 2025/02/09 16:37:19 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _fd(-1) {} //fd -1 means not connected

Client::Client(int fd) : _fd(fd), _nickname(""), _username("") {}

Client::~Client() {}

int Client::getFd() const
{
    return _fd;
}

std::string Client::getNickname() const
{
    return _nickname;
}

std::string Client::getUsername() const
{
    return _username;
}

void Client::setNickname(const std::string &nickname)
{
    _nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
    _username = username;
}

bool Client::isAuthenticated() const
{
    return _authenticated;
}

void Client::setAuthenticated(bool status)
{
    _authenticated = status;
}

bool Client::isConnected() const { return _fd != -1; }
