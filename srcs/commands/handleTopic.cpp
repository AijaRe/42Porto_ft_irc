/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleTopic.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 20:30:33 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/26 21:01:08 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleTopic(Client &client, std::istringstream &iss)
{
    std::string channel_key, new_topic;

    iss >> channel_key;
    if (channel_key.empty())
    {
        std::vector<std::string> params;
        params.push_back("TOPIC");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        return ;
    }
   
    std::getline(iss, new_topic); //capture the rest of the line like new_topic
    new_topic = trimWhitespace(new_topic);
    //Remove the leading colon from realname if it exists
    if (!new_topic.empty() && new_topic[0] == ':') {
        new_topic = new_topic.substr(1);
    }
    
    Channel *chan = _server.getChannelByName(channel_key);
    if (!chan)
    {
        std::vector<std::string> params;
        params.push_back(channel_key);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL, params);
        return ;
    }
    // send the topic to the client
    if (new_topic.empty())
    {
        chan->sendChannelTopic(client);
        return ;
    }
    // setTopic already checks if the client is allowed to set the topic
    if (chan->setTopic(new_topic, &client))
    {
        // send the topic to all users in the channel
        chan->broadcastMessage(chan->getClients(), ":" + client.getNickname() + " TOPIC " + chan->getChannelKey() + " :" + new_topic + "\r\n", client, true);
    } 
    else 
    {
        std::vector<std::string> params;
        params.push_back(channel_key);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_CHANOPRIVSNEEDED, params);
    }
}


