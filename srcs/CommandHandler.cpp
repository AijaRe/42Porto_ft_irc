#include "CommandHandler.hpp"
#include "Server.hpp"
#include "Channel.hpp"


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
    else if (cmd == "TEST3")
        handleTest3();
    /*else if (cmd == "TEST")
        handleTest(client, iss);
    else if (cmd == "TEST2")
        handleTest2(client, iss);*/
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
    std::string channel_name;
    iss >> channel_name;
    

    if (channel_name.empty()) {
        sendError(client.getFd(), "ERROR: No channel given\n");
        return;
    }
    this->_server.addChannel(client, channel_name);
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


/*
void CommandHandler::handleTest(Client &client, std::istringstream &iss)
{
    (void)client;
    (void)iss;

    std::cout << "PRINTING FULL composition of all channels" << std::endl;
    for (size_t i = 0; i < this->_server._channels.size(); i++)
    {
        std::cout << "CHANNEL [" << i << "] " << this->_server._channels[i].getName() << std::endl;
        for (size_t j = 0; j < this->_server._channels[i]._clients.size(); j++)
            std::cout << "\tUSER [" << j << "] " << this->_server._channels[i]._clients[j]->getNickname() << std::endl;

    }

}

void CommandHandler::handleTest2(Client &client, std::istringstream &iss)
{
    (void)client;
    (void)iss;

    std::cout << "PRINTING ADMINS of all channels" << std::endl;
    for (size_t i = 0; i < this->_server._channels.size(); i++)
    {
        std::cout << "CHANNEL [" << i << "] " << this->_server._channels[i].getName() << std::endl;
        for (size_t j = 0; j < this->_server._channels[i]._admins.size(); j++)
            std::cout << "\tADM [" << j << "] " << this->_server._channels[i]._admins[j]->getNickname() << std::endl;

    }

}*/

void CommandHandler::handleTest3()
{
    std::cout << "PRINTING CHANNEL LIST" << std::endl;

    std::map<std::string, Channel>::iterator it;
    for (it = this->_server._channels.begin(); it != this->_server._channels.end(); it++)
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() << std::endl;
}
