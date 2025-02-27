/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelSettings.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:39:08 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/27 16:36:05 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

void Channel::setMode(char mode, bool enable)
{
    size_t pos = this->_modes.find(mode);//procura o modo na string de modos
    if (enable && pos == std::string::npos)//se o modo não estiver presente e for para ativar
        this->_modes += mode;//adiciona o modo
    else if (!enable && pos != std::string::npos)//se o modo estiver presente e for para desativar
        this->_modes.erase(std::remove(_modes.begin(), _modes.end(), mode), _modes.end());//remove o modo
    
}

void Channel::setModePass(const std::string &modepass)
{
    if (modepass.empty())
        _modepass.clear();//remove a senha quando -k é passado
    else
        this->_modepass = modepass;
}

void Channel::setModeLimit(int limit)
{
    if (limit < 0)
        _userlimit = -1;//remove o limite de usuários quando -l é passado
    else
        _userlimit = limit;//adiciona o limite de usuários quando +l é passado
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

void Channel::addOperator(const std::string &nickname)
{
    if(findUserByName(_clients, nickname) == NULL)
        return;
    const Client* client = findUserByName(_clients, nickname);
    if (client)
        _ops[nickname] = client;
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

