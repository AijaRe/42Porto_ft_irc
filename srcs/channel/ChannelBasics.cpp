/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelBasics.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:39 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/22 12:44:10 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
//#include "Server.hpp"

Channel::Channel()
{

}

Channel::Channel(Client &client, const std::string &name, const int &type)
        : _name(name.substr(1)), _topic(""), _created_by(client.getNickname()), _type(type)
{
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

std::string Channel::getTopic()
{
    return this->_topic;
}

void    Channel::setTopic(std::string &new_topic)
{
    this->_topic = new_topic;
}