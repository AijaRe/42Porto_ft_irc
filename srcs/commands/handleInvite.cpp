/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInvite.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 16:17:50 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/27 16:31:32 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleInvite(Client &client, std::istringstream &iss)//INVITE <nickname> <channel>
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

    if (!channel->isOperator(client.getNickname()))//verifica se o usuário é operador do canal
    {        
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_CHANOPRIVSNEEDED);
        return;
    }

    //existe forma mais limpa de buscar por esse usuário?
    std::map<std::string, const Client*> clientMap;
    std::map<int, Client> &clients = _server.getClients();
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        clientMap[it->second.getNickname()] = &(it->second);
    }
    
    const Client *targetClient = channel->findUserByName(clientMap, targetNickname);
    if (!targetClient)//verifica se o usuário existe
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHNICK);
        return;
    }
    
    if (channel->isInvited(targetNickname))//verifica se o usuário já foi convidado
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_USERONCHANNEL);
        return;
    }
    
    channel->addInvitedUser(targetNickname);
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_INVITING);//esse reply precisa ser aprimorado
}
   
