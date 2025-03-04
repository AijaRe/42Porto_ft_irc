/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleModes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:07:55 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/04 17:25:00 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

// RPL_UMODEIS - If <modestring> is not given
//If <target> is a nickname that does not exist on the network, the ERR_NOSUCHNICK (401)
//If <target> is a different nick than the user who sent the command, the ERR_USERSDONTMATCH (502)
//If one or more modes sent are not implemented on the server, the server MUST apply the modes that are implemented, and then send the ERR_UMODEUNKNOWNFLAG (501) in reply along with the MODE message.
// ERR_NOSUCHCHANNEL (403) If <target> is a channel that does not exist on the network
// RPL_CHANNELMODEIS (324) If <modestring> is not given
//If <modestring> is given, the user sending the command MUST have appropriate channel privileges on the target channel to change the modes given. If a user does not have appropriate privileges to change modes on the target channel, the server MUST NOT process the message, and ERR_CHANOPRIVSNEEDED (482)
//Parameters: <target> [<modestring> [<mode arguments>...]]

void CommandHandler::handleMode(Client &client, const std::vector<std::string> &params)
{
    std::string target = params[0]; // Nome do canal
    std::string modes = (params.size() > 1) ? params[1] : ""; // Modificadores de modo (se houver)

    Channel *channel = this->_server.getChannelByName(target);
    if (!channel)
    {
        std::vector<std::string> targetVec(1, target);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL, targetVec);
        return;
    }
    
    if (params.size() == 1)
    {
        std::string modeMessage = ":" + _server.getName() + " 324 " + client.getNickname() + " " + target + " :" + channel->getModes();
        sendDirectMessage(client.getFd(), modeMessage);
        return;
    }
    std::vector<std::string> modeParams(params.begin() + 2, params.end());//Parâmetros dos modos
    
    //DEBUG!!
    /*std::cout << "Lista de parâmetros para modos: ";
    for (size_t i = 0; i < modeParams.size(); ++i)
        std::cout << "[" << modeParams[i] << "] ";
    std::cout << std::endl;*/
    
    processChannelModes(client, channel, modes, modeParams);
}

void CommandHandler::processChannelModes(Client &client, Channel *channel, std::string modes, const std::vector<std::string> &params) {
    bool adding = true;
    std::string responseModes;
    std::string responseParams;

    size_t paramIndex = 0;
    //std::cout << "MODES: " << modes << std::endl;//DEBUG
    for (size_t i = 0; i < modes.size(); ++i)
    {
        char mode = modes[i];
        //std::cout << "MODE: " << mode << std::endl;//DEBUG
        if (mode == '+')
            adding = true;
        else if (mode == '-')
            adding = false;
        else
        {
            std::string param;

            if ((mode == 'k' || mode == 'l' || mode == 'o'))
            {
                if(paramIndex >= params.size())
                {
                    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
                    return;
                }
                param = params[paramIndex];
                paramIndex++;
            }
            switch (mode)
            {
                case 'i': // Modo invite-only
                    channel->setMode('i', adding);
                    responseModes += (adding ? "+i" : "-i");
                    break;

                case 't': // Modo de proteção de tópico
                    channel->setMode('t', adding);
                    responseModes += (adding ? "+t" : "-t");
                    break;

                case 'k': // Modo de senha
                    if (adding)
                    {
                        if (param.empty())
                        {
                            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
                            return;
                        }
                        channel->setModePass(param);
                        channel->setMode('k', true);
                        responseModes += "+k";
                        responseParams += " " + param;
                    }
                    else
                    {
                        channel->setModePass("");
                        channel->setMode('k', false);
                        responseModes += "-k";
                    }
                    break;

                case 'o': // Modo de operador
                    if (param.empty())
                    {
                        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
                        return;
                    }
                    if (adding)
                    {
                        if (!channel->assignOperator(param))
                        {
                            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHNICK);
                            return;
                        }
                    }
                    else
                        channel->removeOperator(param);
                    responseModes += (adding ? "+o" : "-o");
                    responseParams += " " + param;
                    break;

                case 'l': // Modo de limite de usuários
                    if (adding)
                    {
                        if (param.empty())
                        {
                            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
                            return;
                        }
                        int limit = std::atoi(param.c_str());
                        if (limit <= 0)
                        {
                            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_UNKNOWNMODE);
                            return;
                        }
                        channel->setModeLimit(limit);
                        channel->setMode('l', true);
                        responseModes += "+l";
                        responseParams += " " + param;
                    }
                    else if (channel->isOperator(client.getNickname()))
                    {
                        
                        channel->setModeLimit(-1);
                        responseModes += "-l";
                    }
                    else
                        std::cout << "DEFINIR COMO E A MSG PARA UM CLIENT QUE TENTA DAR -l" << std::endl;
                    break;

                default:
                    std::vector<std::string> modeVec(1, std::string(1, mode));
                    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_UNKNOWNMODE, modeVec);
                    return;
            }
        }
    }

    //std::string response = ":" + client.getNickname() + " MODE " + channel->getChannelKey() + " " + responseModes + responseParams + "\r\n";
    std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@" + this->getServer().getName() + " MODE " + channel->getChannelKey() + " " + responseModes + responseParams + "\r\n";
    //>> @time=2025-03-04T16:45:52.382Z :uviana!~uviana1@SpotChat-rns.9p5.48.62.IP MODE #i :+i
    std::cout << "Response: " << response << std::endl;
    //>>:uviana!~uviana@SpotChat-rns.9p5.48.62.IP MODE #ci +o :uviana1
    //nick!~username@host MODE #channel +o :nick_to_get_op
    channel->broadcastMessage(channel->getClients(), response, client, true);
}
