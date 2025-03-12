/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleQuit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:02:09 by arepsa            #+#    #+#             */
/*   Updated: 2025/03/12 20:43:07 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleQuit(Client &client, std::istringstream &iss)
{
	std::string quitMessage;
	std::getline(iss, quitMessage);
	quitMessage = trimWhitespace(quitMessage);
	//remove ":" from the beginning of the message
	if (!quitMessage.empty() && quitMessage[0] == ':')
		quitMessage = quitMessage.substr(1);
	// :<nickname>!<username>@<host> QUIT :Quit: <quit message>
	std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() + " QUIT :Quit:";
	if (!quitMessage.empty())
		message += " " + quitMessage;
	// Broadcast the QUIT message to all channels the user is on
	std::map<std::string, Channel>::iterator it;
	for (it = this->_server.getChannels().begin(); it != this->_server.getChannels().end();)
	{
		if (it->second.isUserOnChannel(client.getNickname()))
		{
			std::string nickname = client.getNickname();
			it->second.broadcastMessage(it->second.getClients(), message, client, false);
			it->second.removeUser(it->second.getClients(), nickname);
		}
		//if user ir the last one in the channel, remove the channel
		if (it->second.getClients().empty())
		{
			// erase the element and return the iterator to the next element
			this->_server.getChannels().erase(it++);
		}
		else
		{
			++it;
		}
	}
	// Mark client for removal and close fd
	_server.markClientForRemoval(client.getFd());
}
