/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlePrivmsg.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:58:52 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/04 11:18:52 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handlePrivmsg(Client &client, std::istringstream &iss)
{
    std::string target, message;
    iss >> target;
    std::getline(iss >> std::ws, message);//message already have the collon in the first position [0].
    
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
            std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@" + this->_server.getName() + " PRIVMSG " + target + " " + message;
            sendDirectMessage(client_found.getFd(), response);
            return;
       }
       else //client não encontrado.
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
            //Falta confirmar as permissões do usuário dentro do channel.
            std::map<std::string, const Client*> clients = channel_found.getClients();
            std::map<std::string, const Client*>::const_iterator it2;
            for (it2 = clients.begin(); it2 != clients.end(); it2++)
            {
                if (it2->first == client.getNickname())
                    continue;
                std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@" + this->_server.getName() + " PRIVMSG " + target + " " + message;
                sendDirectMessage(it2->second->getFd(), response);
            }
        }
        else
        {
            std::string response = ":" + _server.getName() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + target + " :No such channel";
            sendDirectMessage(client.getFd(), response);
        }
    }    
}

