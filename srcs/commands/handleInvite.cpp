/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInvite.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 16:17:50 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/21 16:17:53 by uviana-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

void CommandHandler::handleInvite(Client &client, std::istringstream &iss)
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