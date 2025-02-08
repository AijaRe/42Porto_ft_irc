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
        std::string getName();

        void addClient(const Client &client);
        void addAdmin(const Client &client);
        //PUBLIC FOR TEST PURPOSES
        
        std::vector<const Client *> _clients; //List of clients assigned to this channel.
        std::vector<const Client *> _admins;
    private:
        std::string _name;
        std::string _topic;

        const Client* findClient(std::vector<const Client*> &user_vec, const Client &user) const;

        


};

#endif
