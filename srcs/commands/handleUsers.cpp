/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleUsers.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:12:55 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/07 22:04:06 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void    CommandHandler::welcomeUser(Client &client)
{
    std::vector<std::string> emptyParams;
    
    // 001 <nickname> :Welcome to the WRTM <nickname>!
    std::string nickname_msg = client.getNickname() + "!";
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME, emptyParams, nickname_msg);
    
    // 002 <nickname> :Your host is <server_name>, running version <version>
    std::string server_msg = _server.getName() + ", running version 1.0";
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_YOURHOST, emptyParams, server_msg);
    
    // 003 <nickname> :This server was created <date>
    // convert time to string in dd-mm-yyyy hh:mm:ss format
    std::time_t created_on = _server.getCreatedOn();
    std::tm *server_created = std::localtime(&created_on);
    char server_created_str[20];
    std::strftime(server_created_str, 20, "%d-%m-%Y %H:%M:%S", server_created);
    
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_CREATED, emptyParams, server_created_str);
    
    // 004 <nickname> <server_name> <version> <user_modes> <channel_modes>
    std::string myInfo = ":" + _server.getName() + " " + RPL_MYINFO + " " + client.getNickname() + " " + _server.getName() + " 1.0 o iklt";
    _replies.sendMessage(client.getFd(), myInfo);

    // Send MOTD
    std::string motd_start = _server.getName() + " Message of the day -";
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_MOTDSTART, emptyParams, motd_start);
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_MOTD);
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_ENDOFMOTD);
}


void CommandHandler::handleUser(Client &client, std::istringstream &iss)
{
    std::string username, modeStr, unused, realname;
    iss >> username >> modeStr >> unused;
    std::getline(iss, realname);//capture the rest of the line like realname

    // Clean up the realname by trimming whitespace
    realname = trimWhitespace(realname);
    //Remove the leading colon from realname if it exists
    if (!realname.empty() && realname[0] == ':')
        realname = realname.substr(1);
    
    // Validate the format of the USER command
    if (username.empty() || modeStr.empty() || unused.empty() || realname.empty())
    {
        std::vector<std::string> params;
        params.push_back("USER");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        _replies.sendMessage(client.getFd(), "Usage: USER <username> <mode> <unused> <realname>");
        return;
    }
    
    // Validate the username
    if (!isValidName(username))
    {
        _replies.sendMessage(client.getFd(), "Invalid username");
        return;
    }

   // Validate the mode (should be 0 - modern protocol) 
    if (modeStr != "0" || unused != "*")
    {   
        _replies.sendMessage(client.getFd(), "Only mode 0 is supported and * for unused! Please, try again. [USER <username> <mode> <unused> <realname>]");
        return;
    }

    // Check if the client is already registered
    if (client.passAuthenticated() && !client.getNickname().empty() && !client.getUsername().empty())
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ALREADYREGISTERED);
        return;
    }

    if (client.getNickname().empty())
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NONICKNAMEGIVEN);
        return;
    }
    
    client.setUsername(username);
    client.setRealname(realname);

    logMessage("USER", "Client set username to: " + client.getUsername() + ", realname to: " + client.getRealname());
    if (client.isFullyAuthenticated() && !client.isWelcomed()) {
        welcomeUser(client);
        client.setWelcomed(true);
    }
}

