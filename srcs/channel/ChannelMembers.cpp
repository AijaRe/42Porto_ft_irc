/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelMembers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:37:20 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/04 12:51:28 by arepsa           ###   ########.fr       */
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
        // user format @nickname!username@host - @for operators
        if (this->_ops.find(it->first) != this->_ops.end())
            userListMsg += "@" + it->first;
        else
            userListMsg += it->first;
        }
    sendDirectMessage(client.getFd(), userListMsg);

    // :<server_name> 366 <nickname> <channel> :End of /NAMES list
    std::string endOfList = std::string(":") + SERVER_NAME + " " + RPL_ENDOFNAMES + " " + client.getNickname() + " " + this->getChannelKey() + " :End of /NAMES list.";
    sendDirectMessage(client.getFd(), endOfList);
}

void Channel::sendChannelTopic(Client &client, bool from_topic_cmd)
{
    if (this->_topic.empty() && !from_topic_cmd)
        return ;
    else if (this->_topic.empty() && from_topic_cmd) {
        // NO TOPIC message: :<server_name> 331 <nickname> <channel> :No topic is set
        std::string noTpicMsg = std::string(":") + SERVER_NAME + " " + RPL_NOTOPIC + " " + client.getNickname() + 
        " " + this->getChannelKey() + " :No topic is set";
        sendDirectMessage(client.getFd(), noTpicMsg);
    } else {
        // RPL_TOPIC :<server_name> 332 <nickname> <channel> :<topic>
        std::string topicMsg = std::string(":") + SERVER_NAME + " " + RPL_TOPIC + " " + client.getNickname() +
                            " " + this->getChannelKey() + " :" + this->_topic;
        sendDirectMessage(client.getFd(), topicMsg);
        // RPL_TOPICWHOTIME :<server_name> 333 <nickname> <channel> <author_nick>!<author_usr>@<host> <time>            
        std::string who_time_msg = std::string(":") + SERVER_NAME + " " + RPL_TOPICWHOTIME + " " + 
                            client.getNickname() + " " + this->getChannelKey() + " " + _topic_author->getNickname() + "!" +
                            _topic_author->getUsername() + "@" + _topic_author->getHost() + " " + this->getTopicTime();
        sendDirectMessage(client.getFd(), who_time_msg);
        return ;
    }
}

void Channel::addClient(Client &client)
{
    //Limit mode verification
    if (hasMode('l') && _clients.size() >= _userlimit)
    {
        std::string message = std::string(":") + SERVER_NAME + " " + ERR_CHANNELISFULL + " " + client.getNickname() + " " + this->getChannelKey() + " :Cannot join channel (+l)\r\n";
        sendDirectMessage(client.getFd(), message);
        return;
    }

    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
	ret = this->_clients.insert(std::make_pair(client.getNickname(), &client));
	if (ret.second == false)
    {
        std::string message = std::string(":") + SERVER_NAME + " " + ERR_USERONCHANNEL + " " + 
                            client.getNickname() + " " + this->getName() + " :is already on channel";
        sendDirectMessage(client.getFd(), message);
    }
		
	else
	{
		// JOIN message: :<nickname>!<username>@<host> JOIN <channel> * :<realname>
        // send this message to all users in the channel
        std::string joinMessage = ":" + client.getNickname() + "!~" + client.getUsername() + "@" + client.getHost() +
                        " JOIN " + this->getChannelKey() /* + " * :" + client.getRealname() */;
        std::map<std::string, const Client*>::iterator it;
        for (it = this->_clients.begin(); it != this->_clients.end(); it++)
        {
            sendDirectMessage(it->second->getFd(), joinMessage);
        }
        // Add the channel to the client's joinedChannels set
        client.addChannel(this);
        sendChannelTopic(client, false);
        sendUserList(client);
	}
}

/// @brief Add the client into the channel Op map and send the right messages.
/// @param op Client reference
/// @param is_first_op True to first OP, False the OP privileges was given by another Op.
void Channel::addOp(Client &op, bool is_first_op)
{
    std::pair<std::map<std::string, const Client*>::iterator, bool> ret;
    ret = this->_ops.insert(std::make_pair(op.getNickname(), &op));
    if (ret.second == false)
        sendError(op.getFd(), "The user " + op.getNickname() + "is already OP.\r\n");
    else {
        logMessage("NEW ADMIN", "Congratulations " + op.getNickname() + ", you are admin of the channel " + this->_channel_key);
        // Set user to OP: :nickname!username@host MODE <channel> +o <nickname>
        std::string userListMsg = ":" + op.getNickname() + "!" + op.getUsername() + "@" + op.getHost();
        std::string opMessage = userListMsg + " MODE " + this->getChannelKey() + " +o " + op.getNickname();
        sendDirectMessage(op.getFd(), opMessage);
        //broadcastMessage(this->_clients, opMessage); ????will this be necessary
    }
    if (!is_first_op)
    {
        //:uviana1!~uviana-a1@SpotChat-rns.9p5.48.62.IP MODE #alala +o :uviana11
        //:
        std::cout << "to be done" << std::endl;
    }
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

/// @brief Broadcast a message to all users (optional) inside a map.
/// @param user_map Map to receive the message.
/// @param message Message to be sent.
/// @param issuer Sender of the message.
/// @param broadcast_to_all True to send to all users inside the map, False to ignore the issuer.
void    Channel::broadcastMessage(std::map<std::string, const Client*> &user_map, const std::string &message, Client &issuer, bool broadcast_to_all)
{
    std::map<std::string, const Client*>::iterator it;
    for (it = user_map.begin(); it != user_map.end(); it++)
    {
        if (!broadcast_to_all && it->first == issuer.getNickname())
            continue;
        sendDirectMessage(it->second->getFd(), message);
    }
}
