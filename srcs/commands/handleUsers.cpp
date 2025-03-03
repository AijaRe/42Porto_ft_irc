/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleUsers.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:12:55 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/03 20:48:48 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void    CommandHandler::welcomeUser(Client &client) {
    std::string serverVersion = "1.0";
    // get current timestamp and convert to string
    std::time_t time_now = std::time(0);
    std::ostringstream oss;
    oss << time_now;
    std::string time_now_str = oss.str();

    // 001 <nickname> :Welcome to the WRTM <nickname>!
    std::vector<std::string> params1;
    params1.push_back(client.getNickname() + "!");
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME, params1);

    // 002 <nickname> :Your host is <server_name>, running version <version>
    std::vector<std::string> params2;
    params2.push_back(_server.getName() + ", running version " + serverVersion);
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_YOURHOST, params2);

    // 003 <nickname> :This server was created <date>
    std::vector<std::string> params3;
    params3.push_back(time_now_str);
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_CREATED, params3);
    
    // 004 <nickname> <server_name> <version> <user_modes> <channel_modes>
    std::string myInfo = ":" + _server.getName() + RPL_MYINFO + " " + _server.getName() + " " + serverVersion + " o iklot";
    _replies.sendMessage(client.getFd(), myInfo);

    // Send MOTD
    std::vector<std::string> motd_start;
    motd_start.push_back(_server.getName() + " Message of the day -");
    _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_MOTDSTART, motd_start);
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

