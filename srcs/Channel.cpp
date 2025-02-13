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
    //Set the creator of the channel.
    addClient(client);
    //addOp(client);
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

void Channel::addClient(const Client &client)
{

     //Inserting the channel in the server._channels map.
    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
    ret = this->_clients.insert(std::make_pair(client.getNickname(), &client));
    if (ret.second == false)
        sendError(client.getFd(), "The user " + client.getNickname() + "is already part of " + this->getName() +".\n");
    else
        std::cout << "SUCESS addition" << std::endl;


    /*
    if (findUserByName(this->_clients, client.getNickname()) == NULL)
    {
        std::cout << "\n\n\t\tUSER [" << client.getNickname() << "] NAO econtrado!" << std::endl;


        const Client *client_ptr = &client;
        this->_clients[client.getNickname()] = client_ptr;
        logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + _name);
        sendReturn(client.getFd(), "Congratulations! "+ client.getNickname() + " you joined the channel " + this->getName() + ".\n");

    }
    else
    {
        sendError(client.getFd(), "You are already part of "+ this->getName() + " channel.\n");
        std::cout << "\n\n\t\tUSER [" << client.getNickname() << "] econtrado!" << std::endl;
    }*/
}

void Channel::addOp(const Client &op)
{
    if (findUser(this->_ops, op) == NULL)
    {
        const Client *op_ptr = &op;
        this->_ops[op.getNickname()] = op_ptr;
        logMessage("NEW ADMIN", "Congratulations " + op.getNickname() + ", you are admin of the channel " + _name);
    }
    else
        sendError(op.getFd(), "You are already admin of " + this->getName() + "channel.\n");
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