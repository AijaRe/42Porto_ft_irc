/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 20:31:34 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/26 11:16:54 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Replies.hpp"

Replies::Replies() {
    ircReplies[ERR_UNKNOWNERROR] = "Unknown error occurred";
    ircReplies[ERR_NONICKNAMEGIVEN] = "No nickname given";
    ircReplies[ERR_NICKNAMEINUSE] = "Nickname is already in use";
    ircReplies[ERR_NICKCOLLISION] = "Nickname collision";
    ircReplies[ERR_NEEDMOREPARAMS] = "Not enough parameters";
    ircReplies[ERR_INVITEONLYCHAN] = "Cannot join channel (+i)";
    ircReplies[ERR_USERNOTINCHANNEL] = "User not in channel";
    ircReplies[ERR_CHANOPRIVSNEEDED] = "You're not a channel operator";
    ircReplies[ERR_NOORIGIN] = "No origin specified";
    ircReplies[ERR_ERRONEUSNICKNAME] = "Erroneous nickname";
    ircReplies[ERR_ALREADYREGISTERED] = "You may not reregister";
    ircReplies[ERR_TOOMANYCHANNELS] = "You have joined too many channels";
    ircReplies[ERR_BADCHANNELKEY] = "Cannot join channel (+k)";
    ircReplies[ERR_NOSUCHCHANNEL] = "No such channel";
    ircReplies[ERR_CHANNELISFULL] = "Cannot join channel (+l)";
    ircReplies[ERR_BADCHANMASK] = "Invalid channel name";
    ircReplies[ERR_NOPRIVS] = "No privileges";
    ircReplies[ERR_PASSWDMISMATCH] = "Password incorrect";
    ircReplies[ERR_KEYSET] = "Channel key already set";
    ircReplies[ERR_KICKEDFROMCHAN] = "You have been kicked from the channel";
    ircReplies[ERR_NOCHANMODES] = "Channel doesn't support modes";
    ircReplies[ERR_CANNOTCHANGECHANMODE] = "Cannot change channel mode";
    ircReplies[ERR_CANNOTCHANGEUMODE] = "Cannot change user mode";
    ircReplies[ERR_UNKNOWNMODE] = "Unknown mode";
    ircReplies[ERR_USERONCHANNEL] = "User is already on channel";
    ircReplies[ERR_NOSUCHNICK] = "No such nick/channel";
    ircReplies[ERR_CANNOTSENDTOCHAN] = "Cannot send to channel";
    ircReplies[ERR_NORECIPIENT] = "No recipient given";
    ircReplies[ERR_NOTREGISTERED] = "You have not registered";
    ircReplies[ERR_USERSDONTMATCH] = "Users don't match";
    ircReplies[ERR_UMODEUNKNOWNFLAG] = "Unknown user mode flag";

    ircReplies[RPL_WELCOME] = "Welcome to the We Read The Manual Netwok!";
    //ircReplies[RPL_TOPIC] = "Channel topic is";
    ircReplies[RPL_NOTOPIC] = "No topic is set";
    ircReplies[RPL_INVITING] = "Inviting user to channel";
    ircReplies[RPL_YOUREOPER] = "You are now an operator";
    ircReplies[RPL_CHANNELMODEIS] = "Channel mode is";
    ircReplies[RPL_UMODEIS] = "User mode is";
    
}


//:<servidor> <código numérico> <nickname> :<mensagem textual>

/*void Replies::sendIrcError(int fd, const Server &server, const std::string &nickname, const std::string &errorCode, const std::string &additionalMessage)
{
    std::string targetNickname = nickname.empty() ? "[user_not_registered]" : nickname;

    std::map<std::string, std::string>::const_iterator it = ircReplies.find(errorCode);
    if (it == ircReplies.end())
    {
        it = ircReplies.find("400");
        if (it == ircReplies.end()) {
            return;
        }
    }

    std::string errorMessage = ":" + server.getName() + " " + errorCode + " " + targetNickname + " :" + it->second;
    if (!additionalMessage.empty()) {
        errorMessage += " " + additionalMessage;
    }
    errorMessage += "\r\n";

    sendMessage(fd, errorMessage);
}*/

/* 
** Message format:
** :<server_name> <code> <nickname> <params> :<default_reply_text> <additional_essage>
*/
void Replies::sendIrcInfo(int fd, const Server &server, const std::string &nickname, const std::string &code, const std::vector<std::string> &params, const std::string &additionalMessage) {
    std::string targetNickname = nickname.empty() ? "*" : nickname;

    std::map<std::string, std::string>::const_iterator it = ircReplies.find(code);
    if (it == ircReplies.end()) {
        it = ircReplies.find(ERR_UNKNOWNERROR);
        if (it == ircReplies.end()) {
            return;
        }
    }
    std::string ircMessage = ":" + server.getName() + " " + code + " " + targetNickname;

    // Adiciona os parâmetros
    for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); ++it) {
    ircMessage += " " + *it;
    }

    ircMessage += " :" + it->second;
    if (!additionalMessage.empty()) {
        ircMessage += " " + additionalMessage;
    }
    ircMessage += "\r\n";

    sendMessage(fd, ircMessage);
}

void Replies::sendMessage(int fd, const std::string &message) {
    send(fd, message.c_str(), message.length(), 0);
}
