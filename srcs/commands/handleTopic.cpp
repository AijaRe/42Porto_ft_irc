/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleTopic.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 20:30:33 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/18 20:30:36 by uviana-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleTopic(Client &client, std::istringstream &iss)
{
    std::string channel;
    (void) client;

    iss >> channel;
    if (channel.empty())
    {
        std::cout << "461: NEED MORE PARAMS" << std::endl;
        return ;
    }

    
}

