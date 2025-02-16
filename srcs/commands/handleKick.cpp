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

/*void CommandHandler::handleKick(Client &client, std::istringstream &iss)
{

            Numeric Replies:

                ERR_NEEDMOREPARAMS (461)
                ERR_NOSUCHCHANNEL (403)
                ERR_CHANOPRIVSNEEDED (482)
                ERR_USERNOTINCHANNEL (441)
                ERR_NOTONCHANNEL (442)

            Deprecated Numeric Reply:

                ERR_BADCHANMASK (476)
        

    (void)client;
    std::string channel_key, user_to_kick, reason;*/

    /*
    
    WORKING ON THIS
    std::vector<std::string> users_to_kick;

    iss >> channel_key;

    while (iss >> user_to_kick)
        users_to_kick.push_back(user_to_kick);
    for (std::vector<std::string>::iterator it = users_to_kick.begin(); it != users_to_kick.end(); it++)
        std::cout << "[" << *it << "]" << std::endl;*/
    
    /*iss >> user_to_kick;
    std::getline(iss >> std::ws, reason); //Can have multiple users to kick.

    if (channel_key.empty() || user_to_kick.empty()){
        //Should this print be sent to the client application? (Hexchat).
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
            if (it->second.findUserByName(it->second.getClients(), user_to_kick))
                if (it->second.removeUser(it->second.getClients(), user_to_kick))
                    std::cout << "Success to delete" << std::endl;
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
}*/

