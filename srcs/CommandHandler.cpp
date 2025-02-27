/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:45:12 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/23 20:14:23 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Replies.hpp"

CommandHandler::CommandHandler(Server &server) : _server(server) {}

CommandHandler::~CommandHandler() {}

const Server &CommandHandler::getServer() const
{
    return this->_server;
}

void CommandHandler::handleCommand(Client &client, const std::string &command)
{
    if (command.empty() || command == "\r" || command == "\n")
        return;
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "CAP") {
        handleCap(client, iss);
        return ;
    }

    if (!isClientAllowedToProceed(client, cmd))
        return;

    else if (cmd == "PASS")
        handlePass(client, iss);   
    else if (cmd == "NICK")
        handleNick(client, iss);
    else if (cmd == "USER")
        handleUser(client, iss);
    else if (cmd == "PRIVMSG")
        handlePrivmsg(client, iss);
    else if (cmd == "JOIN")
        handleJoin(client, iss);
    else if (cmd == "PING")
        handlePing(client, iss);
    else if (cmd == "NOTICE")
        handleNotice(client, iss);
    else if (cmd == "MODE") {
        std::vector<std::string> params;
        std::string param;
        while (iss >> param) {
            params.push_back(param);
        }
        handleMode(client, params);
    }
    else if (cmd == "KICK")
        handleKick(client, iss);
    else if (cmd == "TOPIC")
        handleTopic(client, iss);
    else if (cmd == "INVITE")
        handleInvite(client, iss);
    else if (cmd == "WHO")
        handleWho(client, iss);
    else if (cmd == "TEST")
        handleTest();
    else if (cmd == "TEST2")
        handleTest2();
    else if (cmd == "TEST3")
        handleTest3();
    else if (cmd == "TEST4")
        handleTest4();
    else
        sendError(client.getFd(), "Unknown command: " + cmd);
}

