/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 18:43:47 by yfontene          #+#    #+#             */
/*   Updated: 2025/01/31 19:09:16 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/CommandHandler.hpp"
#include "../incs/Server.hpp"

CommandHandler::CommandHandler(Server &server) : _server(server) {}

void CommandHandler::executeCommand(int client_fd, const std::string &message) {
    std::istringstream iss(message);
    std::string command, params;
    iss >> command; // extract the command
    std::getline(iss, params);

    if (command == "NICK") {
        handleNick(client_fd, params);
    } else if (command == "USER") {
        handleUser(client_fd, params);
    } else if (command == "JOIN") {
        handleJoin(client_fd, params);
    } else if (command == "PRIVMSG") {
        handlePrivMsg(client_fd, params);
    } else {
        sendMessage(client_fd, "ERROR: Unknown command: " + command);
    }
}

void CommandHandler::handleNick(int client_fd, const std::string &params) {
    std::string nick = params.substr(1); //remove leading space

    if (nick.empty()) {
        sendMessage(client_fd, "ERROR: Nickname cannot be empty.");
        return;
    }

    for (std::map<int, User>::iterator it = _server._users.begin(); it != _server._users.end(); ++it) {
        if (it->second.getNick() == nick) {
            sendMessage(client_fd, "ERROR: Nickname is already in use.");
            return;
        }
    }

    _server._users[client_fd].setNick(nick);
    sendMessage(client_fd, "Nickname set to " + nick);
}

void CommandHandler::handleUser(int client_fd, const std::string &params) {
    std::string username = params.substr(1);

    if (username.empty()) {
        sendMessage(client_fd, "ERROR: Username cannot be empty.");
        return;
    }

    _server._users[client_fd].setUsername(username);

    if (_server._users[client_fd].isRegistered()) {
        sendMessage(client_fd, "User registered successfully!");
    } else {
        sendMessage(client_fd, "Username set, but registration incomplete. Set your nickname.");
    }
}

void CommandHandler::handleJoin(int client_fd, const std::string &params) {
    if (!_server._users[client_fd].isRegistered()) {
        sendMessage(client_fd, "ERROR: You must set your nickname and username first.");
        return;
    }

    std::string channelName = params.substr(1);
    if (channelName.empty()) {
        sendMessage(client_fd, "ERROR: Channel name cannot be empty.");
        return;
    }

    sendMessage(client_fd, "Joined channel " + channelName);
}

void CommandHandler::handlePrivMsg(int client_fd, const std::string &params) {
    if (!_server._users[client_fd].isRegistered()) {
        sendMessage(client_fd, "ERROR: You must be registered to send private messages.");
        return;
    }

    std::istringstream iss(params);
    std::string target, message;
    iss >> target;
    std::getline(iss, message);

    if (target.empty() || message.empty()) {
        sendMessage(client_fd, "ERROR: Invalid private message format. Use: PRIVMSG <user> <message>");
        return;
    }

    //simply confirming the sending (we still need to implement the search for the destination!!!!)
    sendMessage(client_fd, "Message to " + target + ": " + message);
}
