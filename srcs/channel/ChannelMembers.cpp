/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelMembers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:37:20 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/27 13:18:08 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

/* 
** send list of users currently joined to the channel, 
** including the client who requested the list
*/
void Channel::sendUserList(Client &client)
{
    // :<server_name> 353 <nickname> = <channel> :<user1> <user2> <user3> ...
    std::string userListMsg = std::string(":") + SERVER_NAME + " " + RPL_NAMREPLY + " " + client.getNickname() + " = " + this->getChannelKey() + " :";
    std::map<std::string, const Client*>::iterator it;
    for (it = this->_clients.begin(); it != this->_clients.end(); it++) {
        //add space before nickname except for the first nickname
        if (it != this->_clients.begin())
            userListMsg += " ";
        // user format @nickname!username@host
        userListMsg += "@" + it->first + "!" + it->second->getUsername() + "@" + it->second->getHost();
        }
    userListMsg += "\r\n";
    send(client.getFd(), userListMsg.c_str(), userListMsg.length(), 0);

    // :<server_name> 366 <nickname> <channel> :End of /NAMES list
    std::string endOfList = std::string(":") + SERVER_NAME + " " + RPL_ENDOFNAMES + " " + client.getNickname() + " " + this->getChannelKey() + " :End of /NAMES list\r\n";
    send(client.getFd(), endOfList.c_str(), endOfList.length(), 0);
}

void Channel::sendChannelTopic(Client &client)
{
    if (this->_topic.empty()) {
        // NO TOPIC message: :<server_name> 331 <nickname> <channel> :No topic is set
        std::string noTpicMsg = std::string(":") + SERVER_NAME + " " + RPL_NOTOPIC + " " + client.getNickname() + 
        " " + this->getChannelKey() + " :No topic is set\r\n";
        send(client.getFd(), noTpicMsg.c_str(), noTpicMsg.length(), 0);
    }
    else {
        // RPL_TOPIC :<server_name> 332 <nickname> <channel> :<topic>
        std::string topicMsg = std::string(":") + SERVER_NAME + " " + RPL_TOPIC + " " + client.getNickname() +
        " " + this->getChannelKey() + " :" + this->_topic + "\r\n";
        send(client.getFd(), topicMsg.c_str(), topicMsg.length(), 0);
        // RPL_TOPICWHOTIME :<server_name> 333 <nickname> <channel> <author_nick>!<author_usr>@<host> <time>            
        std::string who_time_msg = std::string(":") + SERVER_NAME + " " + RPL_TOPICWHOTIME + " " + 
                            client.getNickname() + " " + this->getChannelKey() + " " + _topic_author->getNickname() + "!" +
                            _topic_author->getUsername() + "@" + _topic_author->getHost() + " " + this->getTopicTime() + "\r\n";
        send(client.getFd(), who_time_msg.c_str(), who_time_msg.length(), 0);
        return ;
    }
}

void Channel::addClient(Client &client)
{
    //Adicionando veficação de modos
    if (hasMode('l') && _clients.size() >= _userlimit)
    {
        std::string message = std::string(":") + SERVER_NAME + " " + ERR_CHANNELISFULL + " " + client.getNickname() + " " + this->getChannelKey() + " :Cannot join channel (+l)\r\n";
        send(client.getFd(), message.c_str(), message.length(), 0);
        return;
    }

    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
	ret = this->_clients.insert(std::make_pair(client.getNickname(), &client));
	if (ret.second == false)
    {
        std::string message = std::string(":") + SERVER_NAME + " " + ERR_USERONCHANNEL + " " + 
                            client.getNickname() + " " + this->getName() + " :is already on channel\r\n";
        send(client.getFd(), message.c_str(), message.length(), 0);
    }
		
	else
	{
		logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + this->_channel_key); //it was _name: Uatilla
		// JOIN message: :<nickname>!<username>@<host> JOIN <channel>
		std::string joinMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() +
						" JOIN " + this->getChannelKey() + "\r\n"; //HERE
		send(client.getFd(), joinMessage.c_str(), joinMessage.length(), 0);
        
        // Add the channel to the client's joinedChannels set
        client.addChannel(this);
        sendChannelTopic(client);
        sendUserList(client);
	}
}

void Channel::addOp(Client &op)
{
    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
    ret = this->_ops.insert(std::make_pair(op.getNickname(), &op));
    if (ret.second == false)
        sendError(op.getFd(), "The user " + op.getNickname() + "is already OP .\n");
    else
        logMessage("NEW ADMIN", "Congratulations " + op.getNickname() + ", you are admin of the channel " + this->_channel_key); //it was _name: Uatilla
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

std::set<std::string> &Channel::getInviteList()
{
    return this->_invitedUsers;
}

void    Channel::broadcastMessage(std::map<std::string, const Client*> &user_map, const std::string &message)
{
    std::map<std::string, const Client*>::iterator it;
    for (it = user_map.begin(); it != user_map.end(); it++)
        send(it->second->getFd(), message.c_str(), message.length(), 0);
}
