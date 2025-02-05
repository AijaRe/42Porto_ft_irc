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
    
    
    //The nickname will be the op if the channel doesn't exist.

}

void Channel::addClient(const Client &client)
{
    std::vector<const Client*>::iterator it = std::find(this->_clients.begin(), this->_clients.end(), &client);
    if (it == this->_clients.end())
    {
        const Client *client_ptr = &client;
        this->_clients.push_back(client_ptr);
        logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + _name);
    }
    else
        sendError(client.getFd(), "Your are already part of this channel.\n");

}

Channel::~Channel()
{

}

std::string Channel::getName()
{
    return this->_name;
}