/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleJoin.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:45:35 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/23 19:43:09 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

//JOIN #channel1,#channel2,#channel3 pass1,pass2
void CommandHandler::handleJoin(Client &client, std::istringstream &iss)
{
    std::string channel_names, passwords;
    iss >> channel_names;
    iss >> passwords;
    if (channel_names.empty()) {
        std::vector<std::string> params;
        params.push_back("JOIN");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        return;
    }
    //split the channel_names and passwords
    std::vector<std::string> channel_vec = splitString(channel_names, ',');
    std::vector<std::string> password_vec = splitString(passwords, ',');

    //Join each channel
    for (size_t i = 0; i < channel_vec.size(); i++) {
        std::string channel_name = channel_vec[i];
        std::string password = i < password_vec.size() ? password_vec[i] : "";
        this->_server.addChannel(client, channel_name, password);
    }
}

