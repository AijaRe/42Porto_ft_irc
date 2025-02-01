#include "../incs/Command_handler.hpp"
#include "Server.hpp"

Command_handler::Command_handler(Server &server) : _server(server) {}

Command_handler::~Command_handler() {}

void Command_handler::handle_command(Client &client, const std::string &command)
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

void Command_handler::handleNick(Client &client, std::istringstream &iss)
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

void Command_handler::handleUser(Client &client, std::istringstream &iss)
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

void Command_handler::handlePrivmsg(Client &client, std::istringstream &iss)
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

void Command_handler::handleJoin(Client &client, std::istringstream &iss)
{
    std::string channel;
    iss >> channel;
    
    if (channel.empty()) {
        sendError(client.getFd(), "ERROR: No channel given\n");
        return;
    }

    logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + channel);
}

void Command_handler::handlePing(Client &client, std::istringstream &iss)
{
    std::string response = "PONG " + std::string(iss.str().substr(5));
    send(client.getFd(), response.c_str(), response.length(), 0);
}