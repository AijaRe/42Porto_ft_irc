/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelMembers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:37:20 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/22 11:43:58 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

void Channel::addClient(Client &client)
{
    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
	ret = this->_clients.insert(std::make_pair(client.getNickname(), &client));
	if (ret.second == false) {
        std::string message = std::string(":") + SERVER_NAME + " " + ERR_USERONCHANNEL + " " + 
                            client.getNickname() + " " + this->getName() + " :is already on channel\r\n";
        send(client.getFd(), message.c_str(), message.length(), 0);
    }
		
	else
	{
		logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + _name);
		// JOIN message: :<nickname>!<username>@<host> JOIN <channel>
		std::string joinMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() +
						" JOIN " + this->getName() + "\r\n";
		send(client.getFd(), joinMessage.c_str(), joinMessage.length(), 0);
        
        // Add the channel to the client's joinedChannels set
        client.addChannel(this);
        
        if (this->_topic.empty()) {
            // NO TOPIC message: :<server_name> 331 <nickname> <channel> :No topic is set
            std::string noTpicMsg = std::string(":") + SERVER_NAME + " " + RPL_NOTOPIC + " " + client.getNickname() + 
            " " + this->getName() + " :No topic is set\r\n";
            send(client.getFd(), noTpicMsg.c_str(), noTpicMsg.length(), 0);
        }
        else {
            // TOPIC message: :<server_name> 332 <nickname> <channel> :<topic>
            std::string topicMsg = std::string(":") + SERVER_NAME + " " + RPL_TOPIC + " " + client.getNickname() +
            " " + this->getName() + " :" + this->_topic + "\r\n";
            send(client.getFd(), topicMsg.c_str(), topicMsg.length(), 0);
        }
	}
}

void Channel::addOp(Client &op)
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
    std::map<std::string, const Client*>::iterator it;
    it = user_map.find(user_to_remove);
    if (it != user_map.end())
    {
        const Client *client = it->second;
        user_map.erase(it);
        const_cast<Client*>(client)->removeChannel(this);
        return true;
    }
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

