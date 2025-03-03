/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 13:35:49 by yfontene          #+#    #+#             */
/*   Updated: 2025/03/03 20:27:04 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
#define REPLIES_HPP

#include "Server.hpp"
#include "utils.hpp"

//numeric replies
#define RPL_WELCOME                "001" // Authentication
#define RPL_YOURHOST               "002" // Hostname
#define RPL_CREATED                "003" // Server creation date
#define RPL_MYINFO                 "004" // Server info

#define RPL_UMODEIS                "221" // User mode is
#define RPL_ENDOFWHO               "315" // End of WHO list
#define RPL_CHANNELMODEIS          "324" // MODE <channel> <mode> <param>

#define RPL_NOTOPIC                "331" // TOPIC no topic has been set
#define RPL_TOPIC                  "332" // TOPIC sending the topic of the channel
#define RPL_TOPICWHOTIME           "333" // TOPIC when the topic was set and by whom
#define RPL_INVITING               "341" // INVITE sent to the one doing the inviting
#define RPL_WHOREPLY               "352" // WHO reply
#define RPL_NAMREPLY               "353" // NAMES reply
#define RPL_ENDOFNAMES             "366" // End of NAMES list
#define RPL_MOTDSTART              "375" // Start of MOTD
#define RPL_MOTD                   "372" // MOTD
#define RPL_ENDOFMOTD              "376" // End of MOTD
#define RPL_YOUREOPER              "381" // OPER Success Reply

#define ERR_UNKNOWNERROR           "400"
#define ERR_NOSUCHNICK             "401" // No such nick/channel PRIVMSG / MODE / KICK / WHOIS / WHOWAS / INVITE / TOPIC / NAMES / LIST / ISON
#define ERR_CANNOTSENDTOCHAN       "404" // Cannot send to channel
#define ERR_TOOMANYCHANNELS        "405" // JOIN & PART when a user wants to access multiple channels
#define ERR_NOSUCHCHANNEL          "403" // When the channel doesn't exist // MODE / JOIN / INVITE / TOPIC 
#define ERR_NOORIGIN               "409" // PING with no token
#define ERR_NORECIPIENT            "411" // No recipient given
#define ERR_UNKNOWNCOMMAND         "421" // Unknown command
#define ERR_ERRONEUSNICKNAME       "432" // NICK : Invalid NickName
#define ERR_NICKNAMEINUSE          "433" // NICK when a nickname is already in use
#define ERR_NORULES                "434" // RULES
#define ERR_NICKCOLLISION          "436" // NICK nickname Collision
#define ERR_NONICKNAMEGIVEN        "431" // NICK when Nickname isn't provided
#define ERR_USERNOTINCHANNEL       "441" // KICK when trying to kick a user who isn't in channel 
#define ERR_USERONCHANNEL          "443" // When User is already on channel and is invited to
#define ERR_NOTREGISTERED          "451" // When a user tries to send a message before registering
#define ERR_NEEDMOREPARAMS         "461" // When a command has more or less params
#define ERR_ALREADYREGISTERED      "462" // When a user tries to register again
#define ERR_PASSWDMISMATCH         "464" // Join and Oper
#define ERR_KEYSET                 "467" // Mode +k : case of when the key is already set and the user wants to set it again
#define ERR_CHANNELISFULL          "471" // Join when User wants to join a Mode +l channel and is full
#define ERR_UNKNOWNMODE            "472" // When Mode has something other than itlok
#define ERR_INVITEONLYCHAN         "473" // JOIN When channel has invite Mode on
#define ERR_BADCHANNELKEY          "475" // Join when the channel has key and user gave the wrong key
#define ERR_BADCHANMASK            "476" // JOIN When the channel has a mask
#define ERR_NOCHANMODES            "477" // Mode for lobby
#define ERR_KICKEDFROMCHAN         "470" // Join when the user is kicked & Kick when the user has been kicked
#define ERR_CHANOPRIVSNEEDED       "482" // Returned by commands that need special ChanOp prevs :: KICK,INVITE,MODE,TOPIC
#define ERR_NOPRIVILEGES           "481" // MODE RETURNED BY MODE +o/-o to set a Chanop
#define ERR_NOPRIVS                "491" // MODE RETURNED BY MODE +o/-o to set a Chanop
#define ERR_USERSDONTMATCH         "502" // When the user and the target are different
#define ERR_UMODEUNKNOWNFLAG       "501" // When the user sends a mode that doesn't exist
#define ERR_CANNOTCHANGEUMODE      "973" // When at authentication Mode +i is sent by irssi
#define ERR_CANNOTCHANGECHANMODE   "974" // When Mode fails


class Server;

class Replies{
    public:
        Replies();//for initialization of the map
        void sendIrcError(int fd, const Server &server, const std::string &nickname, const std::string &errorCode, const std::string &additionalMessage = "");
        void sendIrcInfo(int fd, const Server &server, const std::string &nickname, const std::string &code, const std::vector<std::string> &params = std::vector<std::string>(), const std::string &additionalMessage = "");
        void sendMessage(int fd, const std::string &message);
    private:
        std::map<std::string, std::string> ircReplies;
};


#endif
