#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include "Client.hpp"
#include "utils.hpp"

class Server;

class Command_handler
{
    public:
        Command_handler(Server &server);
        ~Command_handler();
        void handle_command(Client &client, const std::string &command);
    private:
        Server &_server;

        void handleNick(Client &client, std::istringstream &iss);
        void handleUser(Client &client, std::istringstream &iss);
        void handlePrivmsg(Client &client, std::istringstream &iss);
        void handleJoin(Client &client, std::istringstream &iss);
        void handlePing(Client &client, std::istringstream &iss);
};

#endif