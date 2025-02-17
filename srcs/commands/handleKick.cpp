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
    (void)client;
    std::string channel_key, reason;
    std::vector<std::string> users_to_kick;

    iss >> channel_key;
    parseTargets(iss, users_to_kick, reason);
    //Verify if no channel was informed or the vector was empty.
    if (channel_key.empty() || users_to_kick.empty())
    {
        std::cout << "Usage: KICK <nick> [reason], kicks the nick from the current channel" << std::endl;
        return;
    }

    //Esse canal existe?
    std::map<std::string, Channel>::iterator it = this->_server.getChannels().find(channel_key);
    if (it != this->_server.getChannels().end())
    {
        //O request partiu de um OP?
        if (it->second.findUserByName(it->second.getOps(), client.getNickname()))
        {
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
        else
            std::cout << "[TO DO] User is not OP." << std::endl;
    }
    else
        std::cout << "[TO DO] The channel wasn't found." << std::endl;
}

//Se o op tentar deletar a si proprio?
    //Ele se kicka e nao ficam OPS


