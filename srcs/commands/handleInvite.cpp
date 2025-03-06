/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInvite.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 16:17:50 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/06 22:16:05 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

//INVITE <nickname> <channel>
void CommandHandler::handleInvite(Client &client, std::istringstream &iss)
{
    std::string targetNickname, channelName;
    if (!(iss >> targetNickname >> channelName))
        return;

    Channel *channel = _server.getChannelByName(channelName);
    if (!channel)
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL);
        return;
    }

    if (!channel->isOperator(client.getNickname()))//check if the client is a channel operator
    {        
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_CHANOPRIVSNEEDED);
        return;
    }

    //is there a better way to look for a client by nickname?
    std::map<std::string, const Client*> clientMap;
    std::map<int, Client> &clients = _server.getClients();
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        clientMap[it->second.getNickname()] = &(it->second);
    }
    
    const Client *targetClient = channel->findUserByName(clientMap, targetNickname);
    if (!targetClient)//check if the target client exists
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHNICK);
        return;
    }
    
    if (channel->isInvited(targetNickname))//check if user was already invited
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_USERONCHANNEL);
        return;
    }
    
    channel->addInvitedUser(targetNickname);
    
    // Message to the client who sent the INVITE
    std::string responseInvite = ":" + _server.getName() + " 341 " + client.getNickname() + " " + targetClient->getNickname() + " :" + channelName;
    _replies.sendMessage(client.getFd(), responseInvite);

    // Message invited client
    std::string responseGuest = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() + " INVITE " + targetClient->getNickname() + " :" + channelName;
    _replies.sendMessage(targetClient->getFd(), responseGuest);

    // Message to the channel
    std::string channelMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() + " INVITE " + targetClient->getNickname() + " :" + channelName;
    channel->broadcastMessage(channel->getClients(), channelMessage, client, true);
}
