#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "utils.hpp"

class Server;
class CommandHandler
{
    public:
        CommandHandler(Server &server);
        ~CommandHandler();
        void handleCommand(Client &client, const std::string &command);
    private:
        Server &_server;

        void handleNick(Client &client, std::istringstream &iss);
        void handleUser(Client &client, std::istringstream &iss);
        void handlePrivmsg(Client &client, std::istringstream &iss);
        void handleJoin(Client &client, std::istringstream &iss);
        void handlePing(Client &client, std::istringstream &iss);
        void handleTest(Client &client, std::istringstream &iss);
        void handleTest2(Client &client, std::istringstream &iss);
};

#endif