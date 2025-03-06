/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleCap.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:15:22 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/06 22:12:51 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleCap(Client &client, std::istringstream &iss)
{
    std::string subcmd;    
    iss >> subcmd;    
    if (subcmd == "LS"){        
        std::string capResponse = std::string(":") + SERVER_NAME + " CAP * LS :"; // Empty capability        
        sendDirectMessage(client.getFd(), capResponse);
    } else if (subcmd == "END") {        
        logMessage("CAP", "Client finished requesting capabilities");    
    } else {        
        logMessage("CAP", "Unknown CAP subcommand: " + subcmd);    
    }
}
