#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Client.hpp"
#include "utils.hpp"
#include "Replies.hpp"
#include "Channel.hpp"

class Server;
class Client;

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
        //Channel _channel;

        void handlePass(Client &client, std::istringstream &iss);
        void handleNick(Client &client, std::istringstream &iss);
        void updateNickname(Client &client, const std::string &newNickname);
        void handleUser(Client &client, std::istringstream &iss);
        void handlePrivmsg(Client &client, std::istringstream &iss);
        void handleJoin(Client &client, std::istringstream &iss);
        void handlePing(Client &client, std::istringstream &iss);
        void handleNotice(Client &client, std::istringstream &iss);
        void handleKick(Client &client, std::istringstream &iss);
        bool kickRequestValidation(Client &client, const std::string &channel_key, 
            const std::vector<std::string> &users_to_kick, std::vector<std::string> &params);
        void executeKick(Client &client, Channel &channel, const std::string &reason,
            std::vector<std::string> &users_to_kick, std::vector<std::string> &params);
        void handleCap(Client &client, std::istringstream &iss);
        void handleMode(Client &client, const std::vector<std::string> &params);
        void processChannelModes(Client &client, Channel *channel, std::string modes, const std::vector<std::string> &params);
        void handleTopic(Client &client, std::istringstream &iss);
        void handleInvite(Client &client, std::istringstream &iss);
        bool inviteValidation(Client &client, const std::string &nick_to_invite,
            const std::string &channel_key, const std::string &test_param, Channel &channel_found);
        void inviteUser(const std::string &nick_to_invite, Client &client, Channel &channel);
        void handleWho(Client &client, std::istringstream &iss);
        void handleTest();
        void handleTest2();
        void handleTest3();
        void handleTest4();
};

#endif
