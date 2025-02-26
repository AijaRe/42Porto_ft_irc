/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleTests.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:05:46 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/22 16:02:57 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleTest()
{

    std::cout << "======== PRINTING FULL composition of all channels" << std::endl;
    std::map<std::string, Channel> channels = this->_server.getChannels();
    std::map<std::string, Channel>::iterator it;
    
    for (it = channels.begin(); it != channels.end(); it++)
    {
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() 
            << " Type: " << it->second.getType() 
            << " Created by:  " << it->second.getCreator() << std::endl
            << " Topic: " << it->second.getTopic() << std::endl
            << " Modes: " << it->second.getModes() << std::endl
            << " Password: " << it->second.getModePass() << std::endl;
        
        std::map<std::string, const Client*> clients = it->second.getClients();
        std::map<std::string, const Client*>::const_iterator it2;
        for (it2 = clients.begin(); it2 != clients.end(); it2++) //it.second is the _channel object.
           std::cout << "\tUSER [" << it2->first << "] " << it2->second->getNickname() << std::endl;
    }
}

void CommandHandler::handleTest2()
{
    std::cout << "======== PRINTING ADMINS of all channels" << std::endl;
    std::map<std::string, Channel> channels = this->_server.getChannels();
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); it++)
    {
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() << std::endl;

         std::map<std::string, const Client*> op = it->second.getOps();
        std::map<std::string, const Client*>::const_iterator it2;
        for (it2 = op.begin(); it2 != op.end(); it2++)
            std::cout << "\tADMIN [" << it2->first << "] " << it2->second->getUsername() << std::endl;
    }
}

void CommandHandler::handleTest3()
{
    std::cout << "======== PRINTING CHANNEL LIST" << std::endl;
    std::map<std::string, Channel> channels = this->_server.getChannels();
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); it++)
        std::cout << "CHANNEL[" << it->first << "] " << it->second.getName() << std::endl;
}

