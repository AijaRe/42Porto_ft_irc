/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleKick.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:17:02 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/16 19:17:03 by uviana-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

/*Numeric Replies:

    ERR_NEEDMOREPARAMS (461)
    ERR_NOSUCHCHANNEL (403)
    ERR_CHANOPRIVSNEEDED (482)
    ERR_USERNOTINCHANNEL (441)
    ERR_NOTONCHANNEL (442)

Deprecated Numeric Reply:

    ERR_BADCHANMASK (476)*/


void parseTargets(std::istringstream &iss, std::vector<std::string> &users_to_kick, std::string &reason)
{
    std::string user_to_kick;

    while (iss >> user_to_kick && !user_to_kick.empty())
    {
        if (user_to_kick[0] == ':') //Check for colon
        {
            reason = user_to_kick.substr(1);
            std::string temp;
            while (iss >> temp)
            reason += " " + temp;
            break;
        }
        if (user_to_kick[user_to_kick.length() - 1] == ',')
            user_to_kick = user_to_kick.substr(0, user_to_kick.length() - 1); //Remove the comma at the end.
        std::istringstream users_stream(user_to_kick);
        std::string single_user;
        while (std::getline(users_stream, single_user, ',')) //Verify if there are comma in the middle of the string.
            if (!single_user.empty())
                users_to_kick.push_back(single_user);
    }
}

void CommandHandler::handleKick(Client &client, std::istringstream &iss)
{
    std::string channel_key, reason;
    std::vector<std::string> users_to_kick, params;
    
    (void)params;

    iss >> channel_key;
    parseTargets(iss, users_to_kick, reason);
    if (channel_key.empty() || users_to_kick.empty())
    {
        params.push_back("KICK");
        _replies.sendIrcInfo(client.getFd(), this->_server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        return;
    }
    //Esse canal existe?
    std::map<std::string, Channel>::iterator it = this->_server.getChannels().find(channel_key);
    if (it == this->_server.getChannels().end())
    {
        params.push_back(channel_key);
        _replies.sendIrcInfo(client.getFd(), this->_server, client.getNickname(), ERR_NOSUCHCHANNEL, params);
        return ;
    }

    //O request está no canal?
    if (!it->second.findUserByName(it->second.getClients(), client.getNickname()))
    {
        params.push_back(channel_key);
        _replies.sendIrcInfo(client.getFd(), this->_server, client.getNickname(), ERR_USERNOTINCHANNEL, params);
        return;
    }

    //O request partiu de um OP?
    if (!it->second.findUserByName(it->second.getOps(), client.getNickname()))
    {
        params.push_back(channel_key);
        _replies.sendIrcInfo(client.getFd(), this->_server, client.getNickname(), ERR_CHANOPRIVSNEEDED, params);
        return;
    }
    //Verificar se o usuário existe dentro do channel desejado.      
    for (std::vector<std::string>::iterator it2 = users_to_kick.begin(); it2 != users_to_kick.end(); it2++)
        if (it->second.findUserByName(it->second.getClients(), *it2))
            if (it->second.removeUser(it->second.getClients(), *it2))
                std::cout << "Success to delete" << std::endl; //If reason is empty add a specific comment.
            else
                std::cout << "Not sucess to delete" << std::endl;
        else
            std::cout << "[TO DO] User not found in this channel." << std::endl;
        

        
}
   /*// Kick each user
    for (std::vector<std::string>::iterator it2 = users_to_kick.begin(); it2 != users_to_kick.end(); it2++) {
        if (it->second.findUserByName(it->second.getClients(), *it2)) {
            it->second.removeUser(it->second.getClients(), *it2);
            std::string kickMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " KICK " + channel_key + " " + *it2 + " :" + reason + "\r\n";
            it->second.broadcastMessage(kickMessage);
        } else {
            std::vector<std::string> params;
            params.push_back(*it2);
            params.push_back(channel_key);
            Replies::sendIrcInfo(client.getFd(), this->_server, client.getNickname(), "441", params, "They aren't on that channel");
        }
    }
}*/