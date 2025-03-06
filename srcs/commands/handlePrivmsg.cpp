/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlePrivmsg.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:58:52 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/06 22:24:58 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

/// @brief Send a message to a target (user or channel)
/// @param client Sender of the message.
/// @param iss Target and message to be delivered.
void CommandHandler::handlePrivmsg(Client &client, std::istringstream &iss)
{
    std::string target, message;
    iss >> target;
    std::getline(iss >> std::ws, message);
    
    if (target.empty() || message.empty())
    {
        std::vector<std::string> params;
        params.push_back("PRIVMSG");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NORECIPIENT, params);
        return;
    }
    int target_type = this->_server.checkChannelType(target); 
    
    if (target_type == 0) //0 for users or errors.
    {
       Client client_found;
       if (this->_server.clientExists(target, client_found))
       {
            std::string response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->_server.getName() + " PRIVMSG " + target + " " + message;
            sendDirectMessage(client_found.getFd(), response);
            return;
       }
       else //client not found.
       {
           std::string response = ":" + _server.getName() + " " + ERR_NOSUCHNICK + " " + client.getNickname() + " " + target + " :No such nick";
           sendDirectMessage(client.getFd(), response);
       }
    }
    else
    {
        Channel channel_found;
        if (this->_server.channelExists(target, channel_found))
        {
            std::map<std::string, const Client*> clients = channel_found.getClients();
            if (channel_found.findUserByName(clients, client.getNickname())) //Check if client is part of the channel.
            {
                std::map<std::string, const Client*>::const_iterator it2;
                for (it2 = clients.begin(); it2 != clients.end(); it2++)
                {
                    if (it2->first == client.getNickname())
                        continue;
                    std::string response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->_server.getName() + " PRIVMSG " + target + " " + message;
                    sendDirectMessage(it2->second->getFd(), response);
                }
            }
            else
            {
                std::string response = ":" + _server.getName() + " " + ERR_CANNOTSENDTOCHAN + " " + client.getNickname() + " " + target + " :You cannot send external messages to this channel whilst the +n (noextmsg) mode is set.";
                sendDirectMessage(client.getFd(), response);
            }
        }
        else
        {
            std::string response = ":" + _server.getName() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + target + " :No such channel";
            sendDirectMessage(client.getFd(), response);
        }
    }    
}
