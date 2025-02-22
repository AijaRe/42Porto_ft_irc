/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleJoin.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:45:35 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/22 15:57:41 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleJoin(Client &client, std::istringstream &iss)
{
    std::string channel_name, password;
    iss >> channel_name;
    iss >> password;
    if (channel_name.empty()) {
        std::vector<std::string> params;
        params.push_back("JOIN");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        return;
    }
    if (password.empty())
        password = "";
    this->_server.addChannel(client, channel_name, password);
}

