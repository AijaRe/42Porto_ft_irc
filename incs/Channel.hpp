/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: uviana-a <uviana-a@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:18 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/04 14:41:20 by uviana-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "utils.hpp"

class Client;

class Channel
{
    public:
        Channel(const Client &client, const std::string &name);
        ~Channel();
        void addClient(const Client &client);
        std::string getName();
        
        //PUBLIC FOR TEST PURPOSES
        std::vector<const Client *> _clients; //List of clients assigned to this channel.
    private:
        std::string _name;
        std::string _topic;
        std::vector<Client *> _admins;

        
        void addAdmin(const Client &client);


};

#endif
