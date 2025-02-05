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

#include "utils.hpp"

Channel::Channel(const Client &client, const std::string &name) : _name(name), _topic("")
{
    
    addClient(client);
    addAdmin(client);

    //OP
    
    //The nickname will be the op if the channel doesn't exist.

}

void Channel::addClient(const Client &client)
{
    if (findClient(this->_clients, client) == NULL)
    {
        const Client *client_ptr = &client;
        this->_clients.push_back(client_ptr);
        logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + _name);
    }
    else
        sendError(client.getFd(), "You are already part of "+ this->getName() + " channel.\n");
}

void Channel::addAdmin(const Client &admin)
{
    if (findClient(this->_admins, admin) == NULL)
    {
        const Client *admin_ptr = &admin;
        this->_admins.push_back(admin_ptr);
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

const Client* Channel::findClient(std::vector<const Client*> &user_vec, const Client &user) const
{
    std::vector<const Client*>::const_iterator it;
    it = std::find(user_vec.begin(), user_vec.end(), &user);
    if (it != user_vec.end())
        return *it;
    return NULL; //user not found in the vector provided.
}