/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleWho.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 19:52:58 by arepsa            #+#    #+#             */
/*   Updated: 2025/03/06 22:27:11 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

//WHO command to respond to query for channel (e.g. WHO #ch1)
void	CommandHandler::handleWho(Client &client, std::istringstream &iss)
{
	std::string channel_name;
	iss >> channel_name;
	
	//Only process WHO for channels (must start with #)
	if (channel_name[0] != '#' || channel_name.empty()) {
		std::vector<std::string> params;
		params.push_back("WHO");
		_replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL, params);
		return;
	}
	//Check if channel exists
	std::map<std::string, Channel> &channels = _server.getChannels();
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);

	if (it == channels.end()) {
		std::vector<std::string> params;
		params.push_back("WHO");
		_replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOSUCHCHANNEL, params);
		return;
	}

	// Send reply to all channel clients
	Channel &channel = it->second;
	std::map<std::string, const Client*> &clients = channel.getClients();
	
	std::map<std::string, const Client*>::iterator it2;
	for (it2 = clients.begin(); it2 != clients.end(); it2++) {
		const Client *user = it2->second;
		//flag H inicates that user is Here, 0 is for hopcount
		//@ indicates that user is an operator
		std::string flags = "H";
		if (channel.getOps().find(user->getNickname()) != channel.getOps().end())
			flags += "@";
		// :<server> 352 <client> <channel> <username> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>
		std::string whoResponse = std::string(":") + SERVER_NAME + " " + RPL_WHOREPLY + " " + client.getNickname() + 
								" " + channel_name + " " + user->getUsername() + " " + user->getHost() + 
								" " + SERVER_NAME + " " + user->getNickname() + " " + flags + " :0 " + user->getRealname();
		sendDirectMessage(client.getFd(), whoResponse);
	}

	//End of WHO list
	std::string endOfWho = std::string(":") + SERVER_NAME + " " + RPL_ENDOFWHO + " " + client.getNickname() + 
						" " + channel_name + " :End of /WHO list.";
	sendDirectMessage(client.getFd(), endOfWho);
}
