/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelMembers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:37:20 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/17 22:01:12 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

void Channel::addClient(const Client &client)
{
    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
	ret = this->_clients.insert(std::make_pair(client.getNickname(), &client));
	if (ret.second == false)
		sendError(client.getFd(), "The user " + client.getNickname() + "is already part of " + this->getName() +".\n");
		//this->_commandHandler->_replies.sendIrcError(client.getFd(), this->_commandHandler->getServer(), client.getNickname(), ERR_USERONCHANNEL);
	else
	{
		logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + _name);
		// JOIN message: :<nickname>!<username>@<host> JOIN <channel>
		std::string joinMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() +
						" JOIN " + this->getName() + "\r\n";
		send(client.getFd(), joinMessage.c_str(), joinMessage.length(), 0);
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

const Client* Channel::findUserByName(const std::map<std::string, const Client*> &user_map, const std::string &name) const
{
    if (user_map.empty())
        return NULL;
    std::map<std::string, const Client*>::const_iterator it;
    for (it = user_map.begin(); it != user_map.end(); it++)
    {
        if ((it->second->getNickname()) == name)
            return it->second;
    }
    return NULL; //user not found in the data structure provided.
}

bool    Channel::removeUser(std::map<std::string, const Client*> &user_map, std::string &user_to_remove)
{
    size_t removed = user_map.erase(user_to_remove);
    if (removed == 1)
        return true;
    return false;
}

std::map<std::string, const Client*> &Channel::getClients()
{
    return this->_clients;
}

std::map<std::string, const Client*> &Channel::getOps()
{
    return this->_ops;
}

void    Channel::broadcastMessage(std::map<std::string, const Client*> &user_map, const std::string &message)
{
    std::map<std::string, const Client*>::iterator it;
    for (it = user_map.begin(); it != user_map.end(); it++)
        send(it->second->getFd(), message.c_str(), message.length(), 0);
}
