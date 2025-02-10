/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:07:41 by yfontene          #+#    #+#             */
/*   Updated: 2025/02/09 19:22:17 by yfontene         ###   ########.fr       */
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

const std::string &Client::getRealname() const {
    return _realname;
}

const std::string &Client::getModes() const {
    return _modes;
}

void Client::setNickname(const std::string &nickname)
{
    _nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
    _username = username;
}


void Client::setRealname(const std::string &realname) {
    _realname = realname;
}

void Client::setMode(char mode, bool enable) {
    size_t pos = _modes.find(mode);
    if (enable && pos == std::string::npos) {
        _modes += mode; // Adiciona o modo se não estiver presente
    } else if (!enable && pos != std::string::npos) {
        _modes.erase(pos, 1); // Remove o modo se estiver presente
    }
}

bool Client::hasMode(char mode) const {
    return _modes.find(mode) != std::string::npos;
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

bool Client::operator==(const Client &compare) const
{
    return this->getNickname() == compare.getNickname();
}
