/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleKick.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:17:02 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/06 22:16:36 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

/// @brief Organize the inputs from the KICK command.
/// @param iss List of users and reason (optional).
/// @param users_to_kick Vector to be filled with users to be kicked.
/// @param reason (optional) reason why the user(s) will be kicked out.
void parseTargets(std::istringstream &iss, std::vector<std::string> &users_to_kick, std::string &reason)
{
    std::string user_to_kick;

    while (iss >> user_to_kick && !user_to_kick.empty())
    {
        if (user_to_kick[0] == ':') //Check for colon (this means its the reason text).
        {
            reason = user_to_kick.substr(1);
            std::string temp;
            while (iss >> temp)
            reason += " " + temp;
            break;
        }
        if (user_to_kick[user_to_kick.length() - 1] == ',')
            user_to_kick = user_to_kick.substr(0, user_to_kick.length() - 1); //Remove the comma at the end of the word.
        std::istringstream users_stream(user_to_kick);
        std::string single_user;
        while (std::getline(users_stream, single_user, ',')) //Verify if there are a comma in the middle of the string, for multiple users.
            if (!single_user.empty())
                users_to_kick.push_back(single_user);
    }
}

/// @brief Validate if the KICK command was passed with all required parameters.
/// @param issuer Client who sent the KICK command.
/// @param channel_key Channel where the users must be kicked.
/// @param users_to_kick List of users to be kicked.
/// @param params Vector used in case of fail during the verification.
/// @return True in case of success, otherwise false.
bool    CommandHandler::kickRequestValidation(Client &issuer, const std::string &channel_key, const std::vector<std::string> &users_to_kick,
            std::vector<std::string> &params)
{
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

/// @brief Execute the KICK command.
/// @param issuer Client who sent the KICK command.
/// @param channel Channel where the users must be kicked.
/// @param reason Why the users will be kicked (Optional).
/// @param users_to_kick List of users to be kicked.
/// @param params Vector used in case of fail during the verification.
void CommandHandler::executeKick(Client &issuer, Channel &channel, const std::string &reason,
            std::vector<std::string> &users_to_kick, std::vector<std::string> &params)
{
    for (std::vector<std::string>::iterator it2 = users_to_kick.begin(); it2 != users_to_kick.end(); it2++)
    {
        if (channel.findUserByName(channel.getClients(), *it2))
        {
            std::string temp;
            temp = reason;
            if (temp.empty())
                temp = issuer.getNickname();
            
            std::string kickMessage = ":" + issuer.getNickname() + "!" + issuer.getUsername() + "@" + issuer.getHost() +
                " KICK " + channel.getChannelKey() + " " + *it2 + " :" + temp;
            
            channel.broadcastMessage(channel.getClients(), kickMessage, issuer, true);
            channel.removeUser(channel.getClients(), *it2);

            //If the user to kick is op, remove from op list too.
            if (channel.findUserByName(channel.getOps(), *it2))
                channel.removeUser(channel.getOps(), *it2);

            //Delete channel if no users inside it.
            if (channel.getClients().size() == 0)
            {
                Channel channel_to_delete;
                if (this->_server.channelExists(channel.getChannelKey(), channel_to_delete))
                    this->_server.getChannels().erase(channel_to_delete.getChannelKey());
            }
        }
        else
        {
            params.push_back(*it2);
            params.push_back(channel.getChannelKey());
            this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_USERNOTINCHANNEL, params);
        }
    }   
}

/// @brief Manage a Kick command from the user.
/// @param issuer User that sent a KICK command.
/// @param iss Targets (channel and users to be kicked), and reason. Syntax: KICK #channel user1, user2, user3 [:reason]
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
