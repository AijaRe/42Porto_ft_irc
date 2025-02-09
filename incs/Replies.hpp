/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 13:35:49 by yfontene          #+#    #+#             */
/*   Updated: 2025/02/09 18:42:00 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string>
#include <map>
#include "Server.hpp"
#include "utils.hpp"

//numeric replies
#define RPL_WELCOME                "001" // Authentication

#define RPL_CHANNELMODEIS          "324" // MODE <channel> <mode> <param>

#define RPL_NOTOPIC                "331" // TOPIC no topic has been set
#define RPL_TOPIC                  "332" // TOPIC sending the topic of the channel
#define RPL_INVITING               "341" // INVITE sent to the one doing the inviting
#define RPL_YOUREOPER              "381" // OPER Success Reply

#define ERR_UNKNOWNERROR           "400"
#define ERR_NOSUCHNICK             "401" // No such nick/channel PRIVMSG / MODE / KICK / WHOIS / WHOWAS / INVITE / TOPIC / NAMES / LIST / ISON
#define ERR_CANNOTSENDTOCHAN       "404" // Cannot send to channel
#define ERR_TOOMANYCHANNELS        "405" // JOIN & PART when a user wants to access multiple channels
#define ERR_NOSUCHCHANNEL          "403" // When the channel doesn't exist // MODE / JOIN / INVITE / TOPIC 
#define ERR_NORECIPIENT            "411" // No recipient given
#define ERR_ERRONEUSNICKNAME       "432" // NICK : Invalid NickName
#define ERR_NICKNAMEINUSE          "433" // NICK when a nickname is already in use
#define ERR_NORULES                "434"
#define ERR_NICKCOLLISION          "436" // NICK nickname Collision
#define ERR_NONICKNAMEGIVEN        "431" // NICK when Nickname isn't provided
#define ERR_USERNOTINCHANNEL       "441" // KICK when trying to kick a user who isn't in channel 
#define ERR_USERONCHANNEL          "443" // When User is already on channel and is invited to
#define ERR_NEEDMOREPARAMS         "461" // When a command has more or less params
#define ERR_ALREADYREGISTERED      "462" 
#define ERR_PASSWDMISMATCH         "464" // Join and Oper
#define ERR_KEYSET                 "467" // Mode +k : case of when the key is already set and the user wants to set it again
#define ERR_CHANNELISFULL          "471" // Join when User wants to join a Mode +l channel and is full
#define ERR_UNKNOWNMODE            "472" // When Mode has something other than itlok
#define ERR_INVITEONLYCHAN         "473" // JOIN When channel has invite Mode on
#define ERR_BADCHANNELKEY          "475" // Join when the channel has key and user gave the wrong key
#define ERR_NOCHANMODES            "477" // Mode for lobby
#define ERR_KICKEDFROMCHAN         "470" // Join when the user is kicked & Kick when the user has been kicked
#define ERR_CHANOPRIVSNEEDED       "482" // Returned by commands that need special ChanOp prevs :: KICK,INVITE,MODE,TOPIC
#define ERR_NOPRIVILEGES           "481" // MODE RETURNED BY MODE +o/-o to set a Chanop
#define ERR_NOPRIVS                "491"
#define ERR_CANNOTCHANGEUMODE      "973" // When at authentication Mode +i is sent by irssi
#define ERR_CANNOTCHANGECHANMODE   "974" // When Mode fails

class Server;

class Replies{
    public:
        Replies();//for initialization of the map
        void sendIrcError(int fd, const Server &server, const std::string &nickname, const std::string &errorCode, const std::string &additionalMessage = "");
        void sendMessage(int fd, const std::string &message);
    private:
        std::map<std::string, std::string> ircReplies;
};


#endif