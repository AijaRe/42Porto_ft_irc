#include "../incs/CommandHandler.hpp"
#include "Server.hpp"

CommandHandler::CommandHandler(Server &server) : _server(server) {}

CommandHandler::~CommandHandler() {}

void CommandHandler::handleCommand(Client &client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    if (cmd == "NICK")
        handleNick(client, iss);
    else if (cmd == "USER")
        handleUser(client, iss);
    else if (cmd == "PRIVMSG")
        handlePrivmsg(client, iss);
    else if (cmd == "JOIN")
        handleJoin(client, iss);
    else if (cmd == "PING")
        handlePing(client, iss);
    else
        sendError(client.getFd(), "Unknown command: " + cmd);
}

void CommandHandler::handleNick(Client &client, std::istringstream &iss)
{
    std::string nickname;
    iss >> nickname;
    if (nickname.empty())
    {
        sendError(client.getFd(), "ERROR: No nickname given\n");
        return;
    }
    for (size_t i = 0; i < _server.getClients().size(); i++) {
        if (_server.getClients()[i].getNickname() == nickname) {
            sendError(client.getFd(), "ERROR: Nickname already in use\n");
            return;
        }
    }
    client.setNickname(nickname);
    logMessage("NICK", nickname);
}

void CommandHandler::handleUser(Client &client, std::istringstream &iss)
{
    std::string username;
    iss >> username;
    if (username.empty())
    {
        sendError(client.getFd(), "ERROR: No username given\n");
        return;
    }
    client.setUsername(username);
    logMessage("USER", "Client set username to: " + username);
}

void CommandHandler::handlePrivmsg(Client &client, std::istringstream &iss)
{
    std::string target, message;
    iss >> target;
    std::getline(iss, message);
    
    if (target.empty() || message.empty()) {
        sendError(client.getFd(), "ERROR: Usage: PRIVMSG <nickname> <message>\n");
        return;
    }

    //find the destination client
    for (size_t i = 0; i < _server.getClients().size(); i++) {
        if (_server.getClients()[i].getNickname() == target) {
            std::string response = "PRIVMSG " + target + " :" + message;
            send(_server.getClients()[i].getFd(), response.c_str(), response.length(), 0);
            return;
        }
    }
}

void CommandHandler::handleJoin(Client &client, std::istringstream &iss)
{
    std::string channel;
    iss >> channel;
    
    if (channel.empty()) {
        sendError(client.getFd(), "ERROR: No channel given\n");
        return;
    }

    logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + channel);
}

void CommandHandler::handlePing(Client &client, std::istringstream &iss)
{
    std::string response = "PONG " + std::string(iss.str().substr(5));
    send(client.getFd(), response.c_str(), response.length(), 0);
}