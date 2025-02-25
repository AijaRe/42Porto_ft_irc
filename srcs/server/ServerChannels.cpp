/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerChannels.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:57:49 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/25 23:02:27 by arepsa           ###   ########.fr       */
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

bool isValidChannelName(std::string &channel_key)
{
	if (channel_key.length() < 2 || channel_key.length() > 50)
		return false;
	if (channel_key.find(' ') != std::string::npos || 
		channel_key.find(',') != std::string::npos ||
		channel_key.find(':') != std::string::npos)
		return false;
	//check for case insensitive names - TO DO
	
	return true;
}

bool Server::checkUserChannelLimit(Client &client, std::string &channel_key)
{
	if (client.joinedChannels.size() >= USR_MAX_CHANNELS)
	{
		std::string messge = ":" + this->getName() + " " + ERR_TOOMANYCHANNELS + " " + client.getNickname() + " " + channel_key + " :You have joined too many channels\r\n";
		send(client.getFd(), messge.c_str(), messge.length(), 0);
		return false;
	}
	return true;
}

bool Server::validChannelName(Client &client, std::string &channel_key)
{
	if (channel_key.empty())
	{
		std::string messge = ":" + this->getName() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " JOIN :Not enough parameters\r\n";
		send(client.getFd(), messge.c_str(), messge.length(), 0);
		return false;
	}
	int channel_type = this->checkChannelType(channel_key);
	if (channel_type == USER)
	{
		std::string messge = ":" + this->getName() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + channel_key + " :No such channel\r\n";
		send(client.getFd(), messge.c_str(), messge.length(), 0);
		return false;
	}
	if (!isValidChannelName(channel_key) || (channel_type != CH_STANDARD))
	{
		std::string messge = ":" + this->getName() + " " + ERR_BADCHANMASK + " " + client.getNickname() + " " + channel_key + " :Invalid channel name\r\n";
		send(client.getFd(), messge.c_str(), messge.length(), 0);
		return false;
	}
	return true;
}

void Server::joinExistingChannel(Client &client, Channel &channel, std::string &channel_key, std::string &channel_password)
{
	//check if channel requires a password
	if (channel.hasMode('k') && channel.getModePass() != channel_password)
	{
		std::string messge = ":" + this->getName() + " " + ERR_BADCHANNELKEY + " " + client.getNickname() + " " + channel_key + " :Cannot join channel (+k)\r\n";
		send(client.getFd(), messge.c_str(), messge.length(), 0);
		return;
	}
	//check if channel is full
	if (channel.hasMode('l') && channel.getClients().size() >= channel.getUserLimit())
	{
		std::string messge = ":" + this->getName() + " " + ERR_CHANNELISFULL + " " + client.getNickname() + " " + channel_key + " :Cannot join channel (+l)\r\n";
		send(client.getFd(), messge.c_str(), messge.length(), 0);
		return;
	}
	// check if user has invite - TO DO

	channel.addClient(client);
}

void Server::createNewChannel(Client &client, std::string &channel_key)
{
	// Check for user permissions to create a channel or not necessary???
	
			
	//Building a new channel here, same as addClient + addOp.
	int channel_type = this->checkChannelType(channel_key);
	Channel new_channel(client, channel_key, channel_type);

	//Inserting the channel in the server._channels map.
	std::pair<std::map<std::string, Channel>::iterator, bool> ret;
	ret = this->_channels.insert(std::make_pair(channel_key, new_channel));
	if (ret.second == false)
		sendError(client.getFd(), "There is already a channel called " + channel_key + ".\n"); //Is this even possible to trigger????
}

//Channel Key = [Key + Channel name], for example #chan1 or !eita or &alochannel
void	Server::addChannel(Client &client, std::string &channel_key, std::string &channel_password)
{
	/*Needs to add a verifications to the channel name, before even joining the channel, here are some errors:
    Error Code	HexChat Error Message	            Description
    DONE461	    Not enough parameters	            The JOIN command requires a channel name, but none was provided.
    DONE403	    No such channel	                    The channel name is invalid (e.g., missing # or contains illegal characters).
    done405	    You have joined too many channels	The user has exceeded the server's limit for the number of joined channels.
    474     	Cannot join channel (+b)	        The user is banned from the channel.
    473	        Cannot join channel (+i)	        The channel is invite-only, and the user has not been invited.
    done475	    Cannot join channel (+k)	        The channel requires a password, and the user provided none or an incorrect one.
    done471	    Cannot join channel (+l)	        The channel is full and cannot accept more users.
    done476		Bad channel mask	                The channel name is invalid (e.g., contains illegal characters).
    done476		Erroneous channel name	            The channel name is invalid (e.g., contains spaces or disallowed characters).
    480	        Joining channels too fast	        The user is joining channels too quickly and is being throttled by the server.
    done443	    is already on channel	            The user is already on the specified channel.
    407     	Too many targets	                The user is trying to join too many channels at once.*/
	if (!validChannelName(client, channel_key)) 
		return;
    if (!checkUserChannelLimit(client, channel_key))
		return;
    
    std::map<std::string, Channel>::iterator it = this->_channels.find(channel_key);
    if (it != this->_channels.end()) {
		Channel &channel = it->second;
        joinExistingChannel(client, channel, channel_key, channel_password);
    } else {
        createNewChannel(client, channel_key);
    }
}
