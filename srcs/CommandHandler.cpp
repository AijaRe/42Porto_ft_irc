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
    else if (cmd == "MODE") {
        std::vector<std::string> params;
        std::string param;
        while (iss >> param) {
            params.push_back(param);
        }
        handleMode(client, params);
    }
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
    // Verifica se o cliente já está autenticado
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
    logMessage("INFO", "Client authenticated");

    if (client.isFullyAuthenticated() && !client.isWelcomed()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME);
        client.setWelcomed(true); // Marca o cliente como tendo recebido a mensagem de boas-vindas
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

//unique nickname having a maximum length of nine (9) characters.
void CommandHandler::handleNick(Client &client, std::istringstream &iss)
{
    std::string nickname;
    iss >> nickname;
    // ERR_NONICKNAMEGIVEN (431) - No nickname provided
    if (nickname.empty())
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NONICKNAMEGIVEN);
        return;
    }
    // ERR_ERRONEUSNICKNAME (432) - Invalid characters or format
    if (nickname.length() > 9 || !isValidNickname(nickname))
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ERRONEUSNICKNAME);
        return;
    }

    //std::map<int, Client> &clients = _server.getClients();
    // ERR_NICKNAMEINUSE (433) - Nickname already in use
    for (std::map<int, Client>::iterator it = _server.getClients().begin(); it != _server.getClients().end(); ++it) {
        if (it->second.getNickname() == nickname) {
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NICKNAMEINUSE);
            return;
        }
    }
    // Change nickname and send confirmation
    updateNickname(client, nickname);

    if (client.isFullyAuthenticated() && !client.isWelcomed()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME);
        client.setWelcomed(true);
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
        std::vector<std::string> params;
        params.push_back("USER");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        _replies.sendMessage(client.getFd(), "Usage: USER <username> <mode> <unused> <realname>");
        return;
    }

    if (!client.passAuthenticated()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ALREADYREGISTERED);
        return;
    }

    if (client.getNickname().empty()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NONICKNAMEGIVEN);
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

    client.setPassAuthenticated(true);
    logMessage("USER", "Client set username to: " + username + ", realname to: " + realname);
}

void CommandHandler::handlePrivmsg(Client &client, std::istringstream &iss)
{
    std::string target, message;
    iss >> target;
    std::getline(iss >> std::ws, message);
    
    if (target.empty() || message.empty())
    {
        std::vector<std::string> params;
        params.push_back("PRIVMSG");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NORECIPIENT, params);

        return;
    }
        
    //Estou considerando que caso seja um user, o nick dele já foi feito o parsing.
    int target_type = this->_server.checkChannelType(target); 
    
    if (target_type == 0) //0 for users or errors.
    {
       Client client_found;
       if (this->_server.clientExists(target, client_found))
       {
            std::string response = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
            send(client_found.getFd(), response.c_str(), response.length(), 0);
            return;
       }
       else //client não encontrado.
            std::cout << "_replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHNICK);" << std::endl;
    }
    else
    {
        Channel channel_found;
        if (this->_server.channelExists(target, channel_found)) //ATENÇÃO A ESSA FUNÇÃO
        {
            //Falta confirmar as permissões do usuário dentro do channel.
            std::map<std::string, const Client*> clients = channel_found.getClients();
            std::map<std::string, const Client*>::const_iterator it2;
            for (it2 = clients.begin(); it2 != clients.end(); it2++)
            {
                std::string response = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
                send(it2->second->getFd(), response.c_str(), response.length(), 0);
            }
        }
        else
            std::cout << "_replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHNICK);" << std::endl;
    }    
}

void CommandHandler::handleJoin(Client &client, std::istringstream &iss)
{
    std::string channel_name;
    iss >> channel_name;
    if (channel_name.empty()) {
        std::vector<std::string> params;
        params.push_back("JOIN");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        return;
    }
    this->_server.addChannel(client, channel_name);
}

/*void CommandHandler::handleKick(Client &client, std::istringstream &iss)
{

            Numeric Replies:

                ERR_NEEDMOREPARAMS (461)
                ERR_NOSUCHCHANNEL (403)
                ERR_CHANOPRIVSNEEDED (482)
                ERR_USERNOTINCHANNEL (441)
                ERR_NOTONCHANNEL (442)

            Deprecated Numeric Reply:

                ERR_BADCHANMASK (476)
        

    (void)client;
    std::string channel_key, user_to_kick, reason;*/

    /*
    
    WORKING ON THIS
    std::vector<std::string> users_to_kick;

    iss >> channel_key;

    while (iss >> user_to_kick)
        users_to_kick.push_back(user_to_kick);
    for (std::vector<std::string>::iterator it = users_to_kick.begin(); it != users_to_kick.end(); it++)
        std::cout << "[" << *it << "]" << std::endl;*/
    
    /*iss >> user_to_kick;
    std::getline(iss >> std::ws, reason); //Can have multiple users to kick.

    if (channel_key.empty() || user_to_kick.empty()){
        //Should this print be sent to the client application? (Hexchat).
        std::cout << "Usage: KICK <nick> [reason], kicks the nick from the current channel" << std::endl;
        return;
    }

    //Esse canal existe?
    std::map<std::string, Channel>::iterator it = this->_server.getChannels().find(channel_key);
    if (it != this->_server.getChannels().end())
    {
        //O request partiu de um OP?
        if (it->second.findUserByName(it->second.getOps(), client.getNickname()))
        {
            //Verificar se o usuário existe dentro do channel desejado.            
            if (it->second.findUserByName(it->second.getClients(), user_to_kick))
                if (it->second.removeUser(it->second.getClients(), user_to_kick))
                    std::cout << "Success to delete" << std::endl;
                else
                    std::cout << "Not sucess to delete" << std::endl;
            else
                std::cout << "[TO DO] User not found in this channel." << std::endl;
        }
        else
            std::cout << "[TO DO] User is not OP." << std::endl;
    }
    else
        std::cout << "[TO DO] The channel wasn't found." << std::endl;
}*/

void CommandHandler::handlePing(Client &client, std::istringstream &iss){    
    std::string token;    
    iss >> token;
    if (token.empty()){        
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOORIGIN);        
        return;    
    }    
    std::string response = ":" + _server.getName() + " PONG " + _server.getName() + " :" + token + "\r\n";    
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

    std::cout << "======== PRINTING FULL composition of all channels" << std::endl;
    std::map<std::string, Channel> channels = this->_server.getChannels();
    std::map<std::string, Channel>::iterator it;
    
    for (it = channels.begin(); it != channels.end(); it++)
    {
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() 
            << " Type: " << it->second.getType() 
            << " Created by:  " << it->second.getCreator() << std::endl;
        
        std::map<std::string, const Client*> clients = it->second.getClients();
        std::map<std::string, const Client*>::const_iterator it2;
        for (it2 = clients.begin(); it2 != clients.end(); it2++) //it.second is the _channel object.
           std::cout << "\tUSER [" << it2->first << "] " << it2->second->getNickname() << std::endl;
    }
}

void CommandHandler::handleTest2()
{
    std::cout << "======== PRINTING ADMINS of all channels" << std::endl;
    std::map<std::string, Channel> channels = this->_server.getChannels();
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); it++)
    {
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() << std::endl;

         std::map<std::string, const Client*> op = it->second.getOps();
        std::map<std::string, const Client*>::const_iterator it2;
        for (it2 = op.begin(); it2 != op.end(); it2++)
            std::cout << "\tADMIN [" << it2->first << "] " << it2->second->getUsername() << std::endl;
    }
}

void CommandHandler::handleTest3()
{
    std::cout << "======== PRINTING CHANNEL LIST" << std::endl;
    std::map<std::string, Channel> channels = this->_server.getChannels();
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); it++)
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() << std::endl;
}



void CommandHandler::handleCap(Client &client, std::istringstream &iss){
    std::string subcmd;    
    iss >> subcmd;    
    if (subcmd == "LS"){        
        std::string capResponse = ":CAP * LS :\r\n"; // Empty capability        
        send(client.getFd(), capResponse.c_str(), capResponse.size(), 0);    
    } else if (subcmd == "END") {        
        logMessage("CAP", "Client finished requesting capabilities");    
    } else {        
        logMessage("CAP", "Unknown CAP subcommand: " + subcmd);    
    }
}

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

    if (!channel->verifyUser(client, channel->getOps()))//verifica se o usuario é um OP
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
