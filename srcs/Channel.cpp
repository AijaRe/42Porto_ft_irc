/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:39 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/04 14:41:40 by uviana-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"


Channel::Channel()
{

}

Channel::Channel(const Client &client, const std::string &name, const int &type)
        : _name(name.substr(1)), _topic(""), _created_by(client.getNickname()), _type(type)
{
    addClient(client);
    addOp(client);
}

Channel::~Channel()
{

}

std::string Channel::getName()
{
    return this->_name;
}

void    Channel::setName(const std::string new_name)
{
    this->_name = new_name;
}

const Client* Channel::findUser(const std::map<std::string, const Client*> &user_map, const Client &user) const
{
    if (user_map.empty())
        return NULL;
    std::map<std::string, const Client*>::const_iterator it;
    for (it = user_map.begin(); it != user_map.end(); it++)
    {
        if (*(it->second) == user)
            return it->second;
    }
    return NULL; //user not found in the data structure provided.
}

const Client* Channel::findUserByName(const std::map<std::string, const Client*> &user_map, const std::string &name) const
{
    if (user_map.empty())
        return NULL;
    std::map<std::string, const Client*>::const_iterator it;
    for (it = user_map.begin(); it != user_map.end(); it++)
    {
        std::cout << "\n\t\t\t Goal: " << name << " Current: " << it->second->getNickname() << std::endl;
        if ((it->second->getNickname()) == name)
        {
            std::cout << "\n\t\t\t\t Found!" << std::endl;
            return it->second;
        }
    }
    return NULL; //user not found in the data structure provided.
}

void Channel::setType(int new_type)
{
    switch (new_type)
    {
        case 1:
            this->_type = CH_STANDARD;
            break;
        case 2:
            this->_type = CH_LOCAL;
            break;
        case 3:
            this->_type = CH_NO_EXT;
            break;
        case 4:
            this->_type = CH_SAFE;
            break;
    }
}

int Channel::getType()
{
    return this->_type;
}

std::string Channel::getCreator()
{
    return this->_created_by;
}

const std::map<std::string, const Client*> &Channel::getClients() const
{
    return this->_clients;
}

const std::map<std::string, const Client*> &Channel::getOps() const
{
    return this->_ops;
}

void Channel::addClient(const Client &client)
{
    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
    ret = this->_clients.insert(std::make_pair(client.getNickname(), &client));
    if (ret.second == false)
        sendError(client.getFd(), "The user " + client.getNickname() + "is already part of " + this->getName() +".\n");
    else
    {
        logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + _name);
        sendReturn(client.getFd(), "Congratulations! "+ client.getNickname() + " you joined the channel " + this->getName() + ".\n");
    }
}

void Channel::addOp(const Client &op)
{
    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
    ret = this->_ops.insert(std::make_pair(op.getNickname(), &op));
    if (ret.second == false)
        sendError(op.getFd(), "The user " + op.getNickname() + "is already OP .\n");
    else
        logMessage("NEW ADMIN", "Congratulations " + op.getNickname() + ", you are admin of the channel " + _name);
}

bool Channel::verifyUser(const Client& user, std::map<std::string, const Client *> map)
{
    if (map.empty())
        return false;
    const std::map<std::string, const Client*> &channel_map = map;
    if (findUser(channel_map, user))
        return true;
    return false;    
}