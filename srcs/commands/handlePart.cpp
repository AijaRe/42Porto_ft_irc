/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlePart.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:45:46 by arepsa            #+#    #+#             */
/*   Updated: 2025/03/09 16:09:16 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handlePart(Client &client, std::istringstream &iss)
{
	std::string channelNames;
	std::string partMessage;
	iss >> channelNames;
	std::getline(iss, partMessage);
	partMessage = trimWhitespace(partMessage);
	// Check if user provided at least one channel name
	if (channelNames.empty())
	{
		_replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, std::vector<std::string>(1, "PART"));
		return;
	}
	// Split the channels by commas	and process each one
	std::vector<std::string> channels = splitString(channelNames, ',');
	for (size_t i = 0; i < channels.size(); i++)
	{
		std::map<std::string, Channel>::iterator it = this->_server.getChannels().find(channels[i]);
    	if (it == this->_server.getChannels().end()) {
			std::vector<std::string> targetVec;
			targetVec.push_back(channels[i]);
			_replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL, targetVec);
        	return ;
		}
		if (!it->second.isUserOnChannel(client.getNickname()))
		{
			std::vector<std::string> targetVec;
			targetVec.push_back(channels[i]);
			_replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOTONCHANNEL, targetVec);
			continue;
		}
		// Send PART message to all users in the channel
		std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHost() + " PART " + channels[i];
		if (!partMessage.empty())
			message += " :" + partMessage;
		it->second.broadcastMessage(it->second.getClients(), message, client, true);
		// Remove the user from the channel
		std::string nickname = client.getNickname();
		it->second.removeUser(it->second.getClients(), nickname);
		// If the user was the last one in the channel, remove the channel
		if (it->second.getClients().size() == 0) {
			Channel channel_to_delete;
			if (this->_server.channelExists(it->second.getChannelKey(), channel_to_delete))
				this->_server.getChannels().erase(channel_to_delete.getChannelKey());
		}
	}
}