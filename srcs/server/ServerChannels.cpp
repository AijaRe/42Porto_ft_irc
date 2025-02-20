/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerChannels.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:57:49 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/20 19:54:43 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

std::map<std::string, Channel>&Server::getChannels(){
	return this->_channels;
}

int		Server::checkChannelType(std::string &channel_name)
{
	char type = channel_name[0];
	switch (type)
    {
        case '#':
            return CH_STANDARD;
        case '&':
            return CH_LOCAL;
        case '+':
            return CH_NO_EXT;
        case '!':
            return CH_SAFE;
        default:
            return USER;
    }
}

//Verifica se o channel está no map e caso positivo retorna true e assina a variavel com o channel encontrado.
bool Server::channelExists(const std::string &channel_key, Channel& channel_found)
{
	if (this->_channels.empty())
		return false;
	std::map<std::string, Channel>::iterator it;
	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if (it->first == channel_key)
		{
			channel_found = it->second;
			return true;
		}
	}
	return false;
}

void	Server::addChannel(const Client &client, std::string &channel_key) //Channel Key = [Key + Channel name], for example #chan1 or !eita or &alochannel
{
	/*Needs to add a verifications to the channel name, before even joining the channel, here are some errors:
    Error Code	HexChat Error Message	            Description
    DONE461	        Not enough parameters	            The JOIN command requires a channel name, but none was provided.
    DONE403	        No such channel	                    The channel name is invalid (e.g., missing # or contains illegal characters).
    405	        You have joined too many channels	The user has exceeded the server's limit for the number of joined channels.
    474     	Cannot join channel (+b)	        The user is banned from the channel.
    473	        Cannot join channel (+i)	        The channel is invite-only, and the user has not been invited.
    475	        Cannot join channel (+k)	        The channel requires a password, and the user provided none or an incorrect one.
    471	        Cannot join channel (+l)	        The channel is full and cannot accept more users.
    476	        Bad channel mask	                The channel name is invalid (e.g., contains illegal characters).
    479	        Erroneous channel name	            The channel name is invalid (e.g., contains spaces or disallowed characters).
    437	        Channel temporarily unavailable 	The channel is temporarily unavailable (e.g., being registered or reserved).
    484	        Your connection is restricted	    The user's connection is restricted and cannot join the channel.
    480	        Joining channels too fast	        The user is joining channels too quickly and is being throttled by the server.
    443	        is already on channel	            The user is already on the specified channel.
    442	        You're not on that channel	        The user is not on the specified channel.
    407     	Too many targets	                The user is trying to join too many channels at once.*/
	int channel_type = this->checkChannelType(channel_key);
	if (channel_type == 0)
	{
		std::string messge = ":" + this->getName() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + channel_key + " :No such channel\r\n";
		send(client.getFd(), messge.c_str(), messge.length(), 0);
		return;
	}
	else
	{
		std::map<std::string, Channel>::iterator it = this->_channels.find(channel_key);
		if (it != this->_channels.end())
			it->second.addClient(client);
		else
		{
			std::cout << "[TO DO] Check user permissions to create this type of channel." << std::endl;
			
			//Building a new channel here, same as addClient + addOp.
			Channel new_channel(client, channel_key, channel_type);

			//Inserting the channel in the server._channels map.
			std::pair<std::map<std::string, Channel>::iterator, bool> ret;
			ret = this->_channels.insert(std::make_pair(channel_key, new_channel));
			if (ret.second == false)
				sendError(client.getFd(), "There is already a channel called " + channel_key + ".\n");
		}
	}
}

