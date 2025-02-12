#include "CommandHandler.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Replies.hpp"

CommandHandler::CommandHandler(Server &server) : _server(server) {}

CommandHandler::~CommandHandler() {}

bool CommandHandler::isClientAllowedToProceed(Client &client, const std::string &cmd){
    if(!client.passAuthenticated())
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
    /*else if (cmd == "KICK")
        handleKick(client, iss);*/
    else if (cmd == "TEST3")
        handleTest3();
    else if (cmd == "TEST")
        handleTest();
    else if (cmd == "TEST2")
        handleTest2();
    else
        sendError(client.getFd(), "Unknown command: " + cmd);
}

void CommandHandler::handlePass(Client &client, std::istringstream &iss)
{
    std::string password;
    iss >> password;

    if (password.empty()) {
        //_replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, std::vector<std::string>(), "PASS");
        
        return;
    }

    if (password != _server.getPassword()) {
        //_replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_PASSWDMISMATCH);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_PASSWDMISMATCH);
        return;
    }

    client.setAuthenticated(true);
    logMessage("INFO", "Client authenticated");

    if (client.isFullyAuthenticated()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME);
    }
}

/* 
** Helper function to validate nickname 
** A nickname cannot contain leading digits
** A nickname can contain only alphanumerical characters, ([]{}), (\), (|) characters
*/
bool isValidNickname(const std::string &nickname)
{
    if (std::isdigit(nickname[0]))
        return false;
    for (size_t i = 0; i < nickname.length(); i++)
    {
        if (!std::isalnum(nickname[i]) && nickname[i] != '[' && nickname[i] != ']' && 
            nickname[i] != '{' && nickname[i] != '}' && nickname[i] != '\\' && 
            nickname[i] != '|')
            return false;
    }
    return true;
}

void    CommandHandler::updateNickname(Client &client, const std::string &newNickname)
{
    std::string oldNickname = client.getNickname();
    std::string user = client.getUsername();
    std::string host = client.getHost();

    client.setNickname(newNickname);
    logMessage("NICK", "You are now known as " + newNickname);

    // send NICK confirmation to client
    std::string message = ":" + oldNickname + "!" + user + "@" + host + " NICK " + newNickname + "\r\n";
    send(client.getFd(), message.c_str(), message.length(), 0);
}

/* 
** Helper function to validate nickname 
** A nickname cannot contain leading digits
** A nickname can contain only alphanumerical characters, ([]{}), (\), (|) characters
*/
bool isValidNickname(const std::string &nickname)
{
    if (std::isdigit(nickname[0]))
        return false;
    for (size_t i = 0; i < nickname.length(); i++)
    {
        if (!std::isalnum(nickname[i]) && nickname[i] != '[' && nickname[i] != ']' && 
            nickname[i] != '{' && nickname[i] != '}' && nickname[i] != '\\' && 
            nickname[i] != '|')
            return false;
    }
    return true;
}

void    CommandHandler::updateNickname(Client &client, const std::string &newNickname)
{
    std::string oldNickname = client.getNickname();
    std::string user = client.getUsername();
    std::string host = client.getHost();

    client.setNickname(newNickname);
    logMessage("NICK", "You are now known as " + newNickname);

    // send NICK confirmation to client
    std::string message = ":" + oldNickname + "!" + user + "@" + host + " NICK " + newNickname + "\r\n";
    send(client.getFd(), message.c_str(), message.length(), 0);
}

//unique nickname having a maximum length of nine (9) characters.
void CommandHandler::handleNick(Client &client, std::istringstream &iss)
{
    std::string nickname;
    iss >> nickname;
    // ERR_NONICKNAMEGIVEN (431) - No nickname provided
    if (nickname.empty())
    {
        //_replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NONICKNAMEGIVEN);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NONICKNAMEGIVEN);
        return;
    }
    // ERR_ERRONEUSNICKNAME (432) - Invalid characters or format
    if (nickname.length() > 9 || !isValidNickname(nickname))
    {
        //_replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_ERRONEUSNICKNAME);
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ERRONEUSNICKNAME);
        return;
    }

    //std::map<int, Client> &clients = _server.getClients();
    // ERR_NICKNAMEINUSE (433) - Nickname already in use
    for (std::map<int, Client>::iterator it = _server.getClients().begin(); it != _server.getClients().end(); ++it) {
        if (it->second.getNickname() == nickname) {
            _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NICKNAMEINUSE);
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NICKNAMEINUSE);
            return;
        }
    }
    // Change nickname and send confirmation
    updateNickname(client, nickname);

    if (client.isFullyAuthenticated()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME);
    }
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

    if (!client.passAuthenticated()) {
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

    if (client.isFullyAuthenticated()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME);
    }
}

/*Return the target type see utils.hpp */
int CommandHandler::targetType(const std::string &target)
{
    switch (target[0])
    {
        case '#':
            return CH_STANDARD;
        case '&':
            return CH_LOCAL;
        case '+':
            return CH_NO_EXT;
        case '!':
            return CH_SAFE;
        default:
            return USER;
    }
}

void CommandHandler::handlePrivmsg(Client &client, std::istringstream &iss)
{
    std::string target, message;
   
    iss >> target;
    std::getline(iss >> std::ws, message);
    
    if (target.empty() || message.empty())
    {
        _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NORECIPIENT);
        return;
    }
        
    //Estou considerando que caso seja um user, o nick dele já foi feito o parsing.
    int target_type = this->_server.checkChannelType(target); //0 for users or errors.
    
    if (target_type == 0)
    {
        //Buscar o destinatário na lista de clientes
        //Caso não seja um user válido deve enviar algum erro?
        std::map<int, Client> &clients = _server.getClients();
        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) 
        {
            if (it->second.getNickname() == target) 
            {
                std::string response = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
                send(it->second.getFd(), response.c_str(), response.length(), 0);
                return;
            }
        }
    }
    else
    {
        Channel channel_found;
        if (this->_server.getChannel(target, channel_found))
        {
            std::map<std::string, const Client*>::const_iterator it2;
            for (it2 = channel_found._clients.begin(); it2 != channel_found._clients.end(); it2++)
            {
                std::string response = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
                send(it2->second->getFd(), response.c_str(), response.length(), 0);
            }
        }
        else
            _replies.sendIrcError(client.getFd(), _server, client.getNickname(), ERR_NOSUCHNICK);

        //Encontrar o channel
        //Confirmar permissões do usuário
        //Caso encontrado enviar mensagem
        //Caso nao erro.
    }    
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

/*
void CommandHandler::handleKick(Client &client, std::istringstream &iss)
{
    std::string user_to_kick;
    iss >> user_to_kick;

    if (user_to_kick.empty()){
        //Return the error log.
        return;
    }
    
}*/

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

void CommandHandler::handleTest()
{

    std::cout << "PRINTING FULL composition of all channels" << std::endl;
    std::map<std::string, Channel>::iterator it;
    for (it = this->_server._channels.begin(); it != this->_server._channels.end(); it++)
    {
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() << " Type: " << it->second.getType() << std::endl;
        std::map<std::string, const Client*>::const_iterator it2;
        for (it2 = it->second._clients.begin(); it2 != it->second._clients.end(); it2++) //it.second is the _channel object.
           std::cout << "\tUSER [" << it2->first << "] " << it2->second->getUsername() << std::endl;
    }
}

void CommandHandler::handleTest2()
{
    std::cout << "PRINTING ADMINS of all channels" << std::endl;
    std::map<std::string, Channel>::iterator it;
    for (it = this->_server._channels.begin(); it != this->_server._channels.end(); it++)
    {
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() << std::endl;
        std::map<std::string, const Client*>::const_iterator it2;
        for (it2 = it->second._ops.begin(); it2 != it->second._ops.end(); it2++)
            std::cout << "\tADMIN [" << it2->first << "] " << it2->second->getUsername() << std::endl;
    }
}

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
