/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleNick.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:03:47 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/24 20:46:18 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

/* 
** Helper function to validate nickname 
** A nickname cannot contain leading digits
** A nickname can contain only alphanumerical characters, ([]{}), (\), (|) characters
*/
bool isValidNickname(const std::string &nickname)
{
    if (std::isdigit(nickname[0]))
        return false;
    for (size_t i = 0; i < nickname.length(); i++)
    {
        if (!std::isalnum(nickname[i]) && nickname[i] != '[' && nickname[i] != ']' && 
            nickname[i] != '{' && nickname[i] != '}' && nickname[i] != '\\' && 
            nickname[i] != '|')
            return false;
    }
    return true;
}

void    CommandHandler::updateNickname(Client &client, const std::string &newNickname)
{
    std::string oldNickname = client.getNickname();
    std::string user = client.getUsername();
    std::string host = client.getHost();

    client.setNickname(newNickname);
    logMessage("NICK", "You are now known as " + newNickname);

    // send NICK confirmation to client
    std::string message;
    if (oldNickname.empty())
        message = ":" + newNickname + "!" + user + "@" + host + " NICK " + newNickname + "\r\n";
    else
        message = ":" + oldNickname + "!" + user + "@" + host + " NICK " + newNickname + "\r\n";
    send(client.getFd(), message.c_str(), message.length(), 0);
}

//unique nickname having a maximum length of nine (9) characters.
void CommandHandler::handleNick(Client &client, std::istringstream &iss)
{
    std::string nickname;
    iss >> nickname;
    // ERR_NONICKNAMEGIVEN (431) - No nickname provided
    if (nickname.empty())
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NONICKNAMEGIVEN);
        return;
    }
    // ERR_ERRONEUSNICKNAME (432) - Invalid characters or format
    if (nickname.length() > 9 || !isValidNickname(nickname))
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ERRONEUSNICKNAME);
        return;
    }

    //std::map<int, Client> &clients = _server.getClients();
    // ERR_NICKNAMEINUSE (433) - Nickname already in use
    for (std::map<int, Client>::iterator it = _server.getClients().begin(); it != _server.getClients().end(); ++it) {
        if (it->second.getNickname() == nickname) {
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NICKNAMEINUSE);
            return;
        }
    }
    // Change nickname and send confirmation
    updateNickname(client, nickname);

    if (client.isFullyAuthenticated() && !client.isWelcomed()) {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), RPL_WELCOME);
        client.setWelcomed(true);
    }
}

