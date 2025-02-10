/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:18 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/09 18:13:56 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "utils.hpp"
#include "Client.hpp"

//class Client;

class Channel
{
    public:
        Channel(const Client &client, const std::string &name);
        ~Channel();
        std::string getName();

        void addClient(const Client &client);
        void addAdmin(const Client &client);
        
        //PUBLIC FOR TEST PURPOSES
        std::map<std::string, const Client *> _clients;
        std::map<std::string, const Client *> _admins;
    private:
        std::string _name;
        std::string _topic;

        const Client* findClient(std::map<std::string, const Client*> &user_map, const Client &user) const;
};

#endif
