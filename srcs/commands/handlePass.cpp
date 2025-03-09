/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlePass.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:01:45 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/09 15:29:04 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

bool CommandHandler::isClientAllowedToProceed(Client &client, const std::string &cmd)
{
    if(!client.passAuthenticated())
    {
        if(cmd != "PASS")
            return false;
        return true;
    }
    if (client.getNickname().empty() && cmd != "NICK" && cmd != "USER")
    {
        if (cmd == "PASS" && client.passAuthenticated())
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ALREADYREGISTERED);
        return false;
    }
    if (client.getUsername().empty() && cmd != "USER" && cmd != "NICK")
    {
        if (cmd == "PASS" && client.passAuthenticated())
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ALREADYREGISTERED);
        return false;
    }

    return true;
}

void CommandHandler::handlePass(Client &client, std::istringstream &iss)
{
    // Checks if the client is already authenticated
    if (client.passAuthenticated()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ALREADYREGISTERED);
        return;
    }

    std::string password;
    iss >> password;

    if (password.empty()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, std::vector<std::string>(), "PASS");
        return;
    }

    if (password != _server.getPassword()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_PASSWDMISMATCH);
        return;
    } 

    client.setPassAuthenticated(true);
}
