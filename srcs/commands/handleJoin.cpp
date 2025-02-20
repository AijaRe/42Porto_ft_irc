/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleJoin.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:45:35 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/20 20:30:37 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleJoin(Client &client, std::istringstream &iss)
{
    std::string channel_name;
    iss >> channel_name;
    if (channel_name.empty()) {
        std::vector<std::string> params;
        params.push_back("JOIN");
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NEEDMOREPARAMS, params);
        return;
    }
    this->_server.addChannel(client, channel_name);
}

