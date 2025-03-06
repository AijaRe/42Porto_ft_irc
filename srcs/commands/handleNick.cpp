/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleNick.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:03:47 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/06 22:23:21 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

/* 
** Helper function to validate nickname 
** A nickname cannot contain leading digits
** A nickname can contain only alphanumerical characters, ([]{}), (\), (|), (_) characters
*/
bool CommandHandler::isValidName(const std::string &name)
{
    if (std::isdigit(name[0]))
        return false;
    for (size_t i = 0; i < name.length(); i++)
    {
        if (!std::isalnum(name[i]) && name[i] != '[' && name[i] != ']' && 
            name[i] != '{' && name[i] != '}' && name[i] != '\\' && 
            name[i] != '|' && name[i] != '_')
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

    // send NICK confirmation to client only if the nickname is being changed
    std::string message;
    if (oldNickname.empty()) {
        return ;
    }
    else
        message = ":" + oldNickname + "!" + user + "@" + host + " NICK " + newNickname;
    sendDirectMessage(client.getFd(), message);
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
    if (nickname.length() > 9 || !isValidName(nickname))
    {
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_ERRONEUSNICKNAME);
        return;
    }
    // ERR_NICKNAMEINUSE (433) - Nickname already in use
    for (std::map<int, Client>::iterator it = _server.getClients().begin(); it != _server.getClients().end(); ++it) {
        if (it->second.getNickname() == nickname) {
            _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NICKNAMEINUSE);
            return;
        }
    }
    // Change nickname and send confirmation
    updateNickname(client, nickname);    
}

