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
        const Server &getServer() const;
        Replies _replies;
    private:
        Server &_server;

        void handlePass(Client &client, std::istringstream &iss);
        void handleNick(Client &client, std::istringstream &iss);
        void handleUser(Client &client, std::istringstream &iss);
        void handlePrivmsg(Client &client, std::istringstream &iss);
        void handleJoin(Client &client, std::istringstream &iss);
        void handlePing(Client &client, std::istringstream &iss);
        void handleNotice(Client &client, std::istringstream &iss);
        //void handleKick(Client &client, std::istringstream &iss);
        void handleCap(Client &client, std::istringstream &iss);
        void handleTest();
        void handleTest2();
        void handleTest3();
        void updateNickname(Client &client, const std::string &newNickname);
};

#endif
