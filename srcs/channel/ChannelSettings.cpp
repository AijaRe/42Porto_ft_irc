/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelSettings.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:39:08 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/07 18:38:32 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

void Channel::setMode(char mode, bool enable)
{
    size_t pos = this->_modes.find(mode);//look for the mode in the string
    if (enable && pos == std::string::npos)//if mode is not present and is to be enabled
        this->_modes += mode;//add the mode
    else if (!enable && pos != std::string::npos)//if mode is present and is to be disabled
        this->_modes.erase(std::remove(_modes.begin(), _modes.end(), mode), _modes.end());//remove the mode
    
}

void Channel::setModePass(const std::string &modepass)
{
    if (modepass.empty())
        _modepass.clear();//remove password when empty string is passed
    else
        this->_modepass = modepass;
}

void Channel::setModeLimit(int limit)
{
    if (limit < 0)
        _userlimit = -1;//remove the limit when -1 is passed
    else
        _userlimit = limit;//add the limit when a positive number is passed
}

bool Channel::hasMode(char mode) const
{
    bool hasMode = this->_modes.find(mode) != std::string::npos;
    return hasMode;
}

std::string Channel::getModes() const
{
    return this->_modes.empty() ? "+" : "+" + this->_modes;
}

std::string Channel::getModePass() const
{
    return this->_modepass;
}

void Channel::addInvitedUser(const std::string &nickname)
{
    _invitedUsers.insert(nickname);
}

void Channel::removeInvitedUser(const std::string &nickname)
{
    _invitedUsers.erase(nickname);
}

bool Channel::isInvited(const std::string &nickname) const
{
    return _invitedUsers.find(nickname) != _invitedUsers.end();
}

bool Channel::isUserOnChannel(const std::string &nickname) const
{
    return _clients.find(nickname) != _clients.end();
}

bool Channel::assignOperator(const std::string &nickname)
{
    if(findUserByName(_clients, nickname) == NULL)
        return false;
    const Client* client = findUserByName(_clients, nickname);
    if (client)
        _ops[nickname] = client;
    return true;
}

void Channel::removeOperator(const std::string &nickname)
{
    _ops.erase(nickname);
}

bool Channel::isOperator(const std::string &nickname) const
{
    return _ops.find(nickname) != _ops.end();
}

void Channel::sendToChannel(const std::string &message, const Server &server)
{
    for(std::map<std::string, const Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
    {
        int client_fd = it->second->getFd();
        _replies.sendIrcInfo(client_fd, server, it->second->getNickname(), "NOTICE", std::vector<std::string>(), message);
    }
}
