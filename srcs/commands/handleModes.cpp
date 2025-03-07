/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleModes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:07:55 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/07 19:31:05 by arepsa           ###   ########.fr       */
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

    std::string target = params[0]; // Channel name
    std::string modes = (params.size() > 1) ? params[1] : ""; // Mode changers (if present)
    Channel *channel = this->_server.getChannelByName(target);
    if (!channel)
    {
        std::vector<std::string> targetVec(1, target);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL, targetVec);
        return;
    }

    // If there are no mode changers, return the current channel modes
    if (params.size() == 1)
    {
        std::string modeMessage = ":" + _server.getName() + " 324 " + client.getNickname() + " " + target + " :" + channel->getModes();
        sendDirectMessage(client.getFd(), modeMessage);
        return;
    }

    std::vector<std::string> modeParams(params.begin() + 2, params.end()); // Mode parameters

    // Check if the client is a channel operator
    if (!channel->isOperator(client.getNickname()))
    {
        std::string msg = ":" + _server.getName() + " 482 " + client.getNickname() + " " + target + " :You're not channel operator\r\n";
        sendDirectMessage(client.getFd(), msg);
        return;
    }

    processChannelModes(client, channel, modes, modeParams);
}

void CommandHandler::passMode(Client &client, Channel *channel, const bool& adding, std::string &param, std::string &responseModes, std::string &responseParams)
{
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
}

void CommandHandler::operatorMode(Client &client, Channel *channel, const bool& adding, std::string &param, std::string &responseModes, std::string &responseParams)
{
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
}

void CommandHandler::limitMode(Client &client, Channel *channel, const bool& adding, std::string &param, std::string &responseModes, std::string &responseParams)
{
    if (adding)
    {
        if (param.empty())
        {
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
            return;
        }
        int limit = std::atoi(param.c_str());
        if (limit < 1 || limit > MAX_CONNECTIONS)
            return;
        // turn limit into string again to pass as response parameter, e.g, 12 from 12abc
        std::stringstream ss;
        ss << limit;
        std::string valid_limit = ss.str();
        
        channel->setModeLimit(limit);
        channel->setMode('l', true);
        responseModes += "+l";
        responseParams += " " + valid_limit;
    }
    else
    {
        channel->setModeLimit(-1);
        channel->setMode('l', false);
        responseModes += "-l";
    }
}

bool CommandHandler::checkParams(char &mode, bool &adding, size_t &paramIndex, Client &client, std::string &param, const std::vector<std::string> &params)
{
    if ((mode == 'k' || mode == 'o') || (mode == 'l' && adding)) // Apenas +l requer parâmetro
    {
        if (paramIndex >= params.size())
        {
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
            return false;
        }
        param = params[paramIndex];
        paramIndex++;
    }
    return true;
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

            if (!checkParams(mode, adding, paramIndex, client, param, params))
                return;
            switch (mode)
            {
                case 'i': // Invites only
                    channel->setMode('i', adding);
                    responseModes += (adding ? "+i" : "-i");
                    break;

                case 't': // Protect topic
                    channel->setMode('t', adding);
                    responseModes += (adding ? "+t" : "-t");
                    break;

                case 'k': // Password aka key
                    passMode(client, channel, adding, param, responseModes, responseParams);
                    break;

                case 'o': // Operator
                    operatorMode(client, channel, adding, param, responseModes, responseParams);
                    break;

                case 'l': // User limit
                    limitMode(client, channel, adding, param, responseModes, responseParams);                    
                    break;

                default:
                    std::vector<std::string> modeVec(1, std::string(1, mode));
                    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_UNKNOWNMODE, modeVec);
                    return;
            }
        }
    }

    std::string response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->getServer().getName() + " MODE " + channel->getChannelKey() + " " + responseModes + responseParams;
    channel->broadcastMessage(channel->getClients(), response, client, true);
}