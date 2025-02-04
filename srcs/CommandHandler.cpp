#include "../incs/CommandHandler.hpp"
#include "../incs/Server.hpp"

CommandHandler::CommandHandler(Server &server) : _server(server) {}

CommandHandler::~CommandHandler() {}

bool CommandHandler::isClientAllowedToProceed(Client &client, const std::string &cmd){
    if(!client.isAuthenticated())
    {
        if(cmd != "PASS")
        {
            sendError(client.getFd(), "You must authenticate first! [PASS <password>]");
            return false;
        }
        return true;
    }
    if (client.getNickname().empty() && cmd != "NICK")
    {
        sendError(client.getFd(), "You must set a nickname first! [NICK <nickname>]");
        return false;
    }
    if (client.getUsername().empty() && cmd != "USER" && cmd != "NICK")
    {
        sendError(client.getFd(), "You must set a username first! [USER <username>]");
        return false;
    }
    return true;
}

void CommandHandler::handleCommand(Client &client, const std::string &command)
{
    if (command.empty() || command == "\r" || command == "\n")
        return;
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (!isClientAllowedToProceed(client, cmd))
        return;

    if (cmd == "PASS")
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
    else
        sendError(client.getFd(), "Unknown command: " + cmd);
}

void CommandHandler::handlePass(Client &client, std::istringstream &iss)
{
    std::string password;
    iss >> password;

    if (password.empty()) {
        sendError(client.getFd(), "password required!!");
        return;
    }

    if (password != _server.getPassword()) {
        sendError(client.getFd(), "password incorrect! Please try again.");
        //close(client.getFd());
        return;
    }

    client.setAuthenticated(true);
    logMessage("INFO", "Client authenticated");
}

void CommandHandler::handleNick(Client &client, std::istringstream &iss)
{
    std::string nickname;
    iss >> nickname;
    if (nickname.empty())
    {
        sendError(client.getFd(), "No nickname given\n");
        return;
    }
    for (size_t i = 0; i < _server.getClients().size(); i++) {
        if (_server.getClients()[i].getNickname() == nickname) {
            sendError(client.getFd(), "Nickname already in use\n");
            return;
        }
    }
    client.setNickname(nickname);
    logMessage("NICK", "Client set nickname to: " + nickname);
}

void CommandHandler::handleUser(Client &client, std::istringstream &iss)
{
    std::string username;
    iss >> username;
    if (username.empty())
    {
        sendError(client.getFd(), "No username given\n");
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
        sendError(client.getFd(), "Usage: PRIVMSG <nickname> <message>\n");
        return;
    }

    //find the destination client
    for (size_t i = 0; i < _server.getClients().size(); i++) {
        if (_server.getClients()[i].getNickname() == target) {
            std::string response = "PRIVMSG from" + client.getNickname() + " :" + message;
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
        sendError(client.getFd(), "No channel given\n");
        return;
    }

    logMessage("JOIN", "Client " + client.getNickname() + " joined channel " + channel);
}

void CommandHandler::handlePing(Client &client, std::istringstream &iss)
{
    std::string response = "PONG " + std::string(iss.str().substr(5));
    send(client.getFd(), response.c_str(), response.length(), 0);
}

//Same as PRIVMSG but if there is no nick it does not return an error
void CommandHandler::handleNotice(Client &client, std::istringstream &iss)
{
    std::string target, message;
    iss >> target;
    std::getline(iss, message);

    if(target.empty() || message.empty())
        return;
    
    for (size_t i = 0; i < _server.getClients().size(); i++)
    {
        if (_server.getClients()[i].getNickname() == target)
        {
            std::string response = "NOTICE from " + client.getNickname() + ": " + message;
            send(_server.getClients()[i].getFd(), response.c_str(), response.length(), 0);
            return;
        }
    }
}
