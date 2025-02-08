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

Channel::Channel(const Client &client, const std::string &name) : _name(name), _topic("")
{
    
    addClient(client);
    addAdmin(client);

}

void Channel::addClient(const Client &client)
{
    if (findClient(this->_clients, client) == NULL)
    {
        const Client *client_ptr = &client;
        this->_clients[client.getNickname()] = client_ptr;
        logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + _name);
        sendReturn(client.getFd(), "Congratulations! "+ client.getNickname() + " you joined the channel " + this->getName() + ".\n");

        /*
        //CORRECT HERE!!!!!
        for (size_t j = 0; j < this->_clients.size(); j++)
            std::cout << "\tUSER [" << j << "] " << this->_clients[j]->getNickname() << std::endl;*/
    }
    else
        sendError(client.getFd(), "You are already part of "+ this->getName() + " channel.\n");
}

void Channel::addAdmin(const Client &admin)
{
    if (findClient(this->_admins, admin) == NULL)//findClient if found return the client obj.
    {
        const Client *admin_ptr = &admin;
        this->_admins[admin.getNickname()] = admin_ptr;
        logMessage("NEW ADMIN", "Congratulations " + admin.getNickname() + ", you are admin of the channel " + _name);
    }
    else
        sendError(admin.getFd(), "You are already admin of " + this->getName() + "channel.\n");
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
