/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleModes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:07:55 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/05 13:07:30 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleMode(Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, std::vector<std::string>(1, "MODE"));
        return;
    }

    std::string target = params[0]; // Nome do canal
    std::string modes = (params.size() > 1) ? params[1] : ""; // Modificadores de modo (se houver)

    Channel *channel = this->_server.getChannelByName(target);
    if (!channel)
    {
        std::vector<std::string> targetVec(1, target);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL, targetVec);
        return;
    }

    // Se não houver modos, apenas retorna os modos atuais do canal
    if (params.size() == 1)
    {
        std::string modeMessage = ":" + _server.getName() + " 324 " + client.getNickname() + " " + target + " :" + channel->getModes();
        sendDirectMessage(client.getFd(), modeMessage);
        return;
    }

    std::vector<std::string> modeParams(params.begin() + 2, params.end()); // Parâmetros dos modos

    // Verifica se o cliente é um operador do canal
    if (!channel->isOperator(client.getNickname()))
    {
        std::string msg = ":" + _server.getName() + " 482 " + client.getNickname() + " " + target + " :You're not channel operator\r\n";
        sendDirectMessage(client.getFd(), msg);
        return;
    }

    // Processa os modos
    processChannelModes(client, channel, modes, modeParams);
}

void CommandHandler::processChannelModes(Client &client, Channel *channel, std::string modes, const std::vector<std::string> &params)
{
    bool adding = true;
    std::string responseModes;
    std::string responseParams;

    size_t paramIndex = 0;

    for (size_t i = 0; i < modes.size(); ++i)
    {
        char mode = modes[i];

        if (mode == '+')
            adding = true;
        else if (mode == '-')
            adding = false;
        else
        {
            std::string param;

            // Verifica se o modo requer um parâmetro
            if ((mode == 'k' || mode == 'o') || (mode == 'l' && adding)) // Apenas +l requer parâmetro
            {
                if (paramIndex >= params.size())
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
                    {
                        channel->removeOperator(param);
                    }
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
                        channel->setModeLimit(limit);
                        channel->setMode('l', true);
                        responseModes += "+l";
                        responseParams += " " + param;
                    }
                    else
                    {
                        channel->setModeLimit(-1);
                        channel->setMode('l', false);
                        responseModes += "-l";
                    }
                    break;

                default:
                    std::vector<std::string> modeVec(1, std::string(1, mode));
                    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_UNKNOWNMODE, modeVec);
                    return;
            }
        }
    }

    std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@" + this->getServer().getName() + " MODE " + channel->getChannelKey() + " " + responseModes + responseParams + "\r\n";
    channel->broadcastMessage(channel->getClients(), response, client, true);
}