/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:07:41 by yfontene          #+#    #+#             */
/*   Updated: 2025/02/12 21:02:04 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _fd(-1) {} //fd -1 means not connected

Client::Client(int fd, const std::string &host) : _fd(fd), _host(host), _nickname(""), _username("") {}

Client::~Client() {}

int Client::getFd() const
{
    return _fd;
}

std::string Client::getHost() const
{
    return _host;
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

bool Client::passAuthenticated() const
{
    return _authenticated;
}

bool Client::isFullyAuthenticated() const {
    return passAuthenticated() && !getNickname().empty() && !getUsername().empty();
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
