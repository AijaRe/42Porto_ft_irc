/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:18 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/14 20:33:49 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "utils.hpp"
#include "Client.hpp"
#include "Replies.hpp"


class Channel
{
    public:
        Channel();
        Channel(const Client &client, const std::string &name, const int &type);
        ~Channel();

        void addClient(const Client &client);
        void addOp(const Client &op); //Channel operators
        bool verifyUser(const Client &user, std::map<std::string, const Client *> map);
        const Client* findUserByName(const std::map<std::string, const Client*> &user_map, const std::string &name) const;


        std::string getName();
        void setName(const std::string new_name);
        std::string getCreator();
        
        int  getType();
        void setType(int new_type);

        const std::map<std::string, const Client*> &getClients() const;
        const std::map<std::string, const Client*> &getOps() const;
    private:
        Replies _replies;
    
        std::string _name;
        std::string _topic;
        std::string _created_by;
        int         _type; //Check utils.hpp
        std::map<std::string, const Client *> _clients;
        std::map<std::string, const Client *> _ops;
        const Client* findUser(const std::map<std::string, const Client*> &user_map, const Client &user) const;
};

#endif
