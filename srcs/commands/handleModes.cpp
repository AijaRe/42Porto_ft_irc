/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleModes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:07:55 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/16 19:07:56 by uviana-a         ###   ########.fr       */
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
void CommandHandler::handleMode(Client &client, const std::vector<std::string> &params) {
    if (params.size() < 2) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
        return;
    }

    std::string channelName = params[0];
    std::string modeString = params[1];
    std::string param = (params.size() > 2) ? params[2] : "";

    std::map<std::string, Channel>::iterator it = _server.getChannels().find(channelName);
    if (it == _server.getChannels().end()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL);
        return;
    }
    Channel* channel = &(it->second);//ponteiro para o canal encontrado.

    if (!channel->findUserByName(channel->getOps(), client.getNickname()))//verifica se o usuario é um OP
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_CHANOPRIVSNEEDED);
        return;
    }

    bool adding = true;
    std::string responseModes;
    std::string responseParams;

    for (size_t i = 0; i < modeString.size(); ++i) {
        char mode = modeString[i];

        if (mode == '+') {
            adding = true;
        } else if (mode == '-') {
            adding = false;
        } else if (mode == 'k') {  // Modo senha
            if (adding) {
                if (param.empty()) {
                    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
                    return;
                }
                channel->setModePass(param);
                responseModes += "+k";
                responseParams += " " + param;
            } else {
                channel->setModePass("");
                responseModes += "-k";
            }
        } else if (mode == 'l') {  // Modo limite de usuários
            if (adding) {
                if (param.empty()) {
                    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
                    return;
                }

                std::stringstream ss(param);
                int limit;
                ss >> limit;
                if (ss.fail() || limit <= 0) {
                    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_UNKNOWNMODE);
                    return;
                }

                channel->setModeLimit(limit);
                responseModes += "+l";
                responseParams += " " + param;
            } else {
                channel->setModeLimit(0);
                responseModes += "-l";
            }
        } else {
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_UNKNOWNMODE, std::vector<std::string>(1, std::string(1, mode)));
        }
    }

    if (!responseModes.empty()) {
        std::string response = ":" + client.getNickname() + " MODE " + channelName + " " + responseModes + responseParams + "\r\n";
        channel->sendToChannel(response, _server);
    }
}

