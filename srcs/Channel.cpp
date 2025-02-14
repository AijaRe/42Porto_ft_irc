/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:39 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/14 20:34:57 by yfontene         ###   ########.fr       */
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
    //Set the creator of the channel.
    addClient(client);
    addOp(client);
}

void Channel::addClient(const Client &client)
{
    if (findClient(this->_clients, client) == NULL)
    {
        const Client *client_ptr = &client;
        this->_clients[client.getNickname()] = client_ptr;
        logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + _name);
        sendReturn(client.getFd(), "Congratulations! "+ client.getNickname() + " you joined the channel " + this->getName() + ".\n");
    }
    else
        sendError(client.getFd(), "You are already part of "+ this->getName() + " channel.\n");
}

void Channel::addOp(const Client &op)
{
    if (findClient(this->_ops, op) == NULL)//findClient if found return the client obj.
    {
        const Client *op_ptr = &op;
        this->_ops[op.getNickname()] = op_ptr;
        logMessage("NEW ADMIN", "Congratulations " + op.getNickname() + ", you are admin of the channel " + _name);
    }
    else
        sendError(op.getFd(), "You are already admin of " + this->getName() + "channel.\n");
}

Channel::~Channel()
{

}

std::string Channel::getName()
{
    return this->_name;
}

const Client* Channel::findClient(std::map<std::string, const Client*> &user_map, const Client &user) const
{
    std::map<std::string, const Client*>::const_iterator it;
    for (it = user_map.begin(); it != user_map.end(); it++)
    {
        if (*(it->second) == user)
            return it->second;
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
