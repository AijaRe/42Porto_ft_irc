#include "CommandHandler.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Replies.hpp"

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
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
        return;
    }

    if (password != _server.getPassword()) {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_PASSWDMISMATCH);
        return;
    }

    client.setAuthenticated(true);
    logMessage("INFO", "Client authenticated");
}

//unique nickname having a maximum length of nine (9) characters.
void CommandHandler::handleNick(Client &client, std::istringstream &iss)
{
    std::string nickname;
    iss >> nickname;
    if (nickname.empty())
    {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NONICKNAMEGIVEN);
        return;
    }
    if (nickname.length() > 9)
    {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_ERRONEUSNICKNAME);
        return;
    }

    //std::map<int, Client> &clients = _server.getClients();
    for (std::map<int, Client>::iterator it = _server.getClients().begin(); it != _server.getClients().end(); ++it) {
        if (it->second.getNickname() == nickname) {
            _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NICKNAMEINUSE);
            return;
        }
    }
    client.setNickname(nickname);
    logMessage("NICK", "Client set nickname to: " + nickname);
}

//Parameters: <user> <mode> <unused> <realname>
/*
   USER guest 0 * :Ronnie Reagan   ; User registering themselves with a
                                   username of "guest" and real name
                                   "Ronnie Reagan".

   USER guest 8 * :Ronnie Reagan   ; User registering themselves with a
                                   username of "guest" and real name
                                   "Ronnie Reagan", and asking to be set
                                   invisible.
*/
void CommandHandler::handleUser(Client &client, std::istringstream &iss)
{
    std::string username, modeStr, unused, realname;
    iss >> username >> modeStr >> unused;
    std::getline(iss >> std::ws, realname);//capture the rest of the line like realname
    if (username.empty() || modeStr.empty() || unused.empty() || realname.empty()) {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, "USER");
        _replies.sendMessage(client.getFd(), "Usage: USER <username> <mode> <unused> <realname>");
        return;
    }

    if (!client.isAuthenticated()) {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_ALREADYREGISTERED);
        return;
    }

    if (client.getNickname().empty()) {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NONICKNAMEGIVEN, ":No nickname given");
        return;
    }

    client.setUsername(username);
    client.setRealname(realname);

    //process the bitmask of mode
    int mode = std::atoi(modeStr.c_str());
    if (mode & 0x04) { // Bit 2 (valor 4): modo 'w'
        client.setMode('w', true);
    }
    if (mode & 0x08) { // Bit 3 (valor 8): modo 'i'
        client.setMode('i', true);
    }

    client.setAuthenticated(true);
    logMessage("USER", "Client set username to: " + username + ", realname to: " + realname);
}

void CommandHandler::handlePrivmsg(Client &client, std::istringstream &iss)
{
    std::string target, message;
    iss >> target;
    std::getline(iss >> std::ws, message);
    
    if (target.empty() || message.empty()) {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NORECIPIENT);
        return;
    }

    // Buscar o destinatário na lista de clientes
    std::map<int, Client> &clients = _server.getClients(); // Pegando referência ao mapa de clientes
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.getNickname() == target) {
            std::string response = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
            send(it->second.getFd(), response.c_str(), response.length(), 0);
            return;
        }
    }

    _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NOSUCHNICK);
}


void CommandHandler::handleJoin(Client &client, std::istringstream &iss)
{
    std::string channel_name;
    iss >> channel_name;
    

    if (channel_name.empty()) {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
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


void CommandHandler::handleCap(Client &client, std::istringstream &iss)
{   
    (void)iss;
    logMessage("CAP", "Client requested capabilities");
    std::string capResponse = "CAP * LS :\r\n";
    send(client.getFd(), capResponse.c_str(), capResponse.size(), 0);

    std::string capEnd = "CAP END\r\n";
    send(client.getFd(), capEnd.c_str(), capEnd.size(), 0);
}
