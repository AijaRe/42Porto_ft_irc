/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleJoin.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:45:35 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/27 11:22:42 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

//JOIN #channel1,#channel2,#channel3 pass1,pass2
void CommandHandler::handleJoin(Client &client, std::istringstream &iss)
{
    std::string channel_names, passwords;
    iss >> channel_names;
    iss >> passwords;
    
    if (channel_names.empty())
    {
        std::vector<std::string> params;
        params.push_back("JOIN");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        return;
    }
    //split the channel_names and passwords
    std::vector<std::string> channel_vec = splitString(channel_names, ',');
    std::vector<std::string> password_vec = splitString(passwords, ',');

    //Join each channel
    for (size_t i = 0; i < channel_vec.size(); i++)
    {
        std::string channel_name = channel_vec[i];
        std::string password = i < password_vec.size() ? password_vec[i] : "";
        
        //adicionado por yasmine:
        Channel *channel = _server.getChannelByName(channel_name);
        if(!channel)
        {
            _server.addChannel(client, channel_name, password);
            continue;
        }
        std::cout << "Tentando entrar no canal: " << channel_name << std::endl;
        //verifica se o modo +k esta ativado e se a senha esta correta
        if(channel->hasMode('k'))
        {
            std::cout << "Modo +k ativado? " << (channel->hasMode('k') ? "true" : "false") << std::endl;
            std::cout << "Senha fornecida: " << password << std::endl;
            std::cout << "Senha do canal: " << channel->getModePass() << std::endl;
            if(password.empty() || password != channel->getModePass())
            {
                std::cout << "Senha incorreta" << std::endl;
                std::vector<std::string> params;
                params.push_back(channel_name);
                _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_BADCHANNELKEY, params);
                continue;
            }
        }
        channel->addClient(client);
        std::cout << "Cliente " << client.getNickname() << " entrou no canal " << channel_name << std::endl;
        std::vector<std::string> params;
        params.push_back(channel_name);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_TOPIC, params);
    }
}

