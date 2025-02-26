/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleCap.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:15:22 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/20 19:12:10 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleCap(Client &client, std::istringstream &iss){
    std::string subcmd;    
    iss >> subcmd;    
    if (subcmd == "LS"){        
        std::string capResponse = std::string(":") + SERVER_NAME + " CAP * LS :\r\n"; // Empty capability        
        send(client.getFd(), capResponse.c_str(), capResponse.size(), 0);    
    } else if (subcmd == "END") {        
        logMessage("CAP", "Client finished requesting capabilities");    
    } else {        
        logMessage("CAP", "Unknown CAP subcommand: " + subcmd);    
    }
}
