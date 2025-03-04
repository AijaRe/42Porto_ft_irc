/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleUsers.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:12:55 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/04 10:08:28 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void    CommandHandler::welcomeUser(Client &client) {
    
    std::vector<std::string> emptyParams;
    // 001 <nickname> :Welcome to the WRTM <nickname>!
    std::string nickname_msg = client.getNickname() + "!";
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME, emptyParams, nickname_msg);
    
    // 002 <nickname> :Your host is <server_name>, running version <version>
    std::string server_msg = _server.getName() + ", running version 1.0";
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_YOURHOST, emptyParams, server_msg);
    
    // 003 <nickname> :This server was created <date>
    // get current timestamp and convert to string in  dd-mm-yyyy format
    std::time_t time_now = std::time(0);
    std::tm *now = std::localtime(&time_now);
    char time_now_str[11];
    std::strftime(time_now_str, 11, "%d-%m-%Y", now);
    
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_CREATED, emptyParams, time_now_str);
    
    // 004 <nickname> <server_name> <version> <user_modes> <channel_modes>
    std::string myInfo = ":" + _server.getName() + " " + RPL_MYINFO + " " + client.getNickname() + " " + _server.getName() + " 1.0 o iklt\r\n";
    _replies.sendMessage(client.getFd(), myInfo);

    // Send MOTD
    std::string motd_start = _server.getName() + " Message of the day -";
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_MOTDSTART, emptyParams, motd_start);
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_MOTD);
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_ENDOFMOTD);
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
    std::getline(iss, realname);//capture the rest of the line like realname

    // Clean up the realname by trimming whitespace
    realname = trimWhitespace(realname);
    //Remove the leading colon from realname if it exists
    if (!realname.empty() && realname[0] == ':') {
        realname = realname.substr(1);
    }
    
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
    if (client.isFullyAuthenticated() && !client.isWelcomed()) {
        welcomeUser(client);
        client.setWelcomed(true);
    }
}

