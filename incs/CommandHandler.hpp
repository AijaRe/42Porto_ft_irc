#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Client.hpp"
#include "utils.hpp"

class Server;

class CommandHandler
{
    public:
        CommandHandler(Server &server);
        ~CommandHandler();
        void handleCommand(Client &client, const std::string &command);
        bool isClientAllowedToProceed(Client &client, const std::string &cmd);
    private:
        Server &_server;

        void handlePass(Client &client, std::istringstream &iss);
        void handleNick(Client &client, std::istringstream &iss);
        void handleUser(Client &client, std::istringstream &iss);
        void handlePrivmsg(Client &client, std::istringstream &iss);
        void handleJoin(Client &client, std::istringstream &iss);
        void handlePing(Client &client, std::istringstream &iss);
        void handleNotice(Client &client, std::istringstream &iss);
};

#endif
