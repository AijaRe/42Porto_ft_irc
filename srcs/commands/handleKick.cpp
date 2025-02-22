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
        while (std::getline(users_stream, single_user, ',')) //Verify if there are a comma in the middle of the string.
            if (!single_user.empty())
                users_to_kick.push_back(single_user);
    }
}

bool    CommandHandler::kickRequestValidation(Client &issuer, const std::string &channel_key, const std::vector<std::string> &users_to_kick,
            std::vector<std::string> &params)
{
    //Is the command correct? 

    //ALERT! HEXCHAT doesn't deal in this way.
    if (channel_key.empty() || users_to_kick.empty())
    {
        params.push_back("KICK");
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_NEEDMOREPARAMS, params);
        return false;
    }
    //Does this channel exists?
    std::map<std::string, Channel>::iterator it = this->_server.getChannels().find(channel_key);
    if (it == this->_server.getChannels().end())
    {
        params.push_back(channel_key);
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_NOSUCHCHANNEL, params);
        return false;
    }

    //The issuer is in the channel?
    if (!it->second.findUserByName(it->second.getClients(), issuer.getNickname()))
    {
        params.push_back(channel_key);
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_USERNOTINCHANNEL, params);
        return false;
    }

    //Is the issuer an OP?
    if (!it->second.findUserByName(it->second.getOps(), issuer.getNickname()))
    {
        params.push_back(channel_key);
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_CHANOPRIVSNEEDED, params);
        return false;
    }
    return true;
}

void CommandHandler::executeKick(Client &issuer, Channel &channel, const std::string &reason,
            std::vector<std::string> &users_to_kick, std::vector<std::string> &params)
{
    for (std::vector<std::string>::iterator it2 = users_to_kick.begin(); it2 != users_to_kick.end(); it2++)
    {
        if (channel.findUserByName(channel.getClients(), *it2))
        {
            //KICK message: :<IssuerNick>!~<IssuerName>@<host> KICK <channel> <TargetNick> :[reason or IssuerNick (if reason blank)]

            //Check if reason is empty.
            std::string temp;
            temp = reason;
            if (temp.empty())
                temp = issuer.getNickname();
            
            std::string kickMessage = ":" + issuer.getNickname() + "!~" + issuer.getUsername() + "@" + issuer.getHost() +
                " KICK " + channel.getChannelKey() + " " + *it2 + " :" + temp + "\r\n";
            
            channel.broadcastMessage(channel.getClients(), kickMessage);
            //Remove the client
            channel.removeUser(channel.getClients(), *it2);
        }
        else
        {
            params.push_back(*it2);
            params.push_back(channel.getChannelKey());
            this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_USERNOTINCHANNEL, params);
        }
    }   
}

void CommandHandler::handleKick(Client &issuer, std::istringstream &iss)
{
    std::string channel_key, reason;
    std::vector<std::string> users_to_kick, params;
    
    (void)params;

    iss >> channel_key;
    parseTargets(iss, users_to_kick, reason);
    if (!kickRequestValidation(issuer, channel_key, users_to_kick, params))
        return;
    
    std::map<std::string, Channel>::iterator it = this->_server.getChannels().find(channel_key);
    if (it == this->_server.getChannels().end())
        return ;

    executeKick(issuer, it->second, reason, users_to_kick, params);  
}