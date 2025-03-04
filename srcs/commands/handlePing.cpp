/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlePing.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:15:05 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/04 10:42:33 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handlePing(Client &client, std::istringstream &iss){    
    std::string token;    
    iss >> token;
    if (token.empty()){        
        _replies.sendIrcInfo(client.getFd(), _server, client.getNickname(), ERR_NOORIGIN);        
        return;    
    }    
    std::string response = ":" + _server.getName() + " PONG " + _server.getName() + " :" + token + "\r\n";
    sendDirectMessage(client.getFd(), response);
}

