#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Client.hpp"
#include "utils.hpp"
#include "Replies.hpp"

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
        Replies _replies;

        void handlePass(Client &client, std::istringstream &iss);
        void handleNick(Client &client, std::istringstream &iss);
        void handleUser(Client &client, std::istringstream &iss);
        void handlePrivmsg(Client &client, std::istringstream &iss);
        void handleJoin(Client &client, std::istringstream &iss);
        void handlePing(Client &client, std::istringstream &iss);
        void handleNotice(Client &client, std::istringstream &iss);
        void handleTest(Client &client, std::istringstream &iss);
        void handleTest2(Client &client, std::istringstream &iss);
        void handleTest3();
};

#endif
