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
		const Server&	getServer() const;
		void		handleCommand(Client &client, const std::string &command);
		bool		isClientAllowedToProceed(Client &client, const std::string &cmd);
		bool		isValidName(const std::string &nickname);
		Replies		_replies;
	private:
		Server &_server;

		//command handlers
		void handleCap(Client &client, std::istringstream &iss);
		void handlePass(Client &client, std::istringstream &iss);
		void handleNick(Client &client, std::istringstream &iss);
		void handleUser(Client &client, std::istringstream &iss);
		void handlePing(Client &client, std::istringstream &iss);
		void handleNotice(Client &client, std::istringstream &iss);
		void handlePrivmsg(Client &client, std::istringstream &iss);
		void handleJoin(Client &client, std::istringstream &iss);
		void handleMode(Client &client, const std::vector<std::string> &params);
		void handleKick(Client &client, std::istringstream &iss);
		void handleTopic(Client &client, std::istringstream &iss);
		void handleInvite(Client &client, std::istringstream &iss);
		void handlePart(Client &client, std::istringstream &iss);
		void handleWho(Client &client, std::istringstream &iss);
		
		//helper functions
		void welcomeUser(Client &client);
		void updateNickname(Client &client, const std::string &newNickname);
		void passMode(Client &client, Channel *channel, const bool& adding, std::string &param, std::string &responseModes, std::string &responseParams);
		void operatorMode(Client &client, Channel *channel, const bool& adding, std::string &param, std::string &responseModes, std::string &responseParams);
		void limitMode(Client &client, Channel *channel, const bool& adding, std::string &param, std::string &responseModes, std::string &responseParams);
		bool checkParams(char &mode, bool &adding, size_t &paramIndex, Client &client, std::string &param, const std::vector<std::string> &params);
		void processChannelModes(Client &client, Channel *channel, std::string modes, const std::vector<std::string> &params);
		bool kickRequestValidation(Client &client, const std::string &channel_key, 
						const std::vector<std::string> &users_to_kick, std::vector<std::string> &params);
		void executeKick(Client &client, Channel &channel, const std::string &reason,
						std::vector<std::string> &users_to_kick, std::vector<std::string> &params);
	
		//test functions
		void handleTest();
		void handleTest2();
		void handleTest3();
		void handleTest4();
};

#endif
