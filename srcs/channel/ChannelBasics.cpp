/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelBasics.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:39 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/01 14:27:56 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
{

}

Channel::Channel(Client &client, const std::string &name, const int &type)
        : _channel_key(name), _name(name.substr(1)), _created_by(client.getNickname()), _type(type),  _topic("")
{
    //set default modes
    setMode('i', false); // Mode +i (channel is invite only)
	setMode('k', false); // Mode +k (channel key/password)
	setMode('l', false); // Mode +l (user limit)
	setMode('o', false); // Mode +o (channel operator)
	setMode('n', true); // Mode +n (no external messages)
    setMode('t', true); // Mode +t (topic set by channel operator only)
    
    addClient(client);
    addOp(client);
}

Channel::~Channel()
{

}

std::string Channel::getName()
{
    return this->_name;
}

std::string Channel::getChannelKey()
{
    return this->_channel_key;
}

unsigned int Channel::getUserLimit()
{
    return this->_userlimit;
}

void    Channel::setName(const std::string &new_name)
{
    this->_name = new_name;
    if (!this->_channel_key.empty())
        this->_channel_key = _channel_key[0] + new_name;

}

void Channel::setType(int new_type)
{
    switch (new_type)
    {
        case 1:
            this->_type = CH_STANDARD;
            this->_channel_key[0] = '#';
            break;
        case 2:
            this->_type = CH_LOCAL;
            this->_channel_key[0] = '&';
            break;
        case 3:
            this->_type = CH_NO_EXT;
            this->_channel_key[0] = '+';
            break;
        case 4:
            this->_type = CH_SAFE;
            this->_channel_key[0] = '!';
            break;
    }
}

int Channel::getType()
{
    return this->_type;
}

std::string Channel::getCreator()
{
    return this->_created_by;
}

std::string Channel::getTopic() const
{
    return this->_topic;
}

const Client* Channel::getTopicAuthor() const
{
    return this->_topic_author;
}

std::string Channel::getTopicTime() const
{
    // Convert _topic_time to string using std::ostringstream
    std::ostringstream oss;
    oss << _topic_time;
    std::string topic_time_str = oss.str();
    return topic_time_str;
}

std::map<std::string, const Client*> &Channel::getClients()
{
    return this->_clients;
}

std::map<std::string, const Client*> &Channel::getOps()
{
    return this->_ops;
}

bool Channel::setTopic(const std::string &new_topic, const Client *author)
{
    //print mode
    logMessage("INFO", "Channel mode is: " + getModes());
    if (hasMode('t') && !isOperator(author->getNickname()))
        return false;
    this->_topic = new_topic;
    this->_topic_author = author;
    this->_topic_time = std::time(NULL);
    return true;
}

