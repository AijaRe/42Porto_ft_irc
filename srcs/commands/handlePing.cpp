/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlePing.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:15:05 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/16 19:15:07 by uviana-a         ###   ########.fr       */
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
    send(client.getFd(), response.c_str(), response.length(), 0);
}

