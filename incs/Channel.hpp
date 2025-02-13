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
        Channel();
        Channel(const Client &client, const std::string &name, const int &type);
        ~Channel();

        void addClient(const Client &client);
        void addOp(const Client &op); //Channel operators
        bool verifyUser(const Client &user, std::map<std::string, const Client *> map);
        const Client* findUserByName(const std::map<std::string, const Client*> &user_map, const std::string &name) const;


        std::string getName();
        void setName(const std::string new_name);
        
        int  getType();
        void setType(int new_type);

        std::string getCreator();

        const std::map<std::string, const Client*> &getClients() const;
        //PUBLIC FOR TEST PURPOSES
        std::map<std::string, const Client *> _clients;
        std::map<std::string, const Client *> _ops;
    private:
        std::string _name;
        std::string _topic;
        std::string _created_by;
        int         _type; //Check utils.hpp
        const Client* findUser(const std::map<std::string, const Client*> &user_map, const Client &user) const;
};

#endif
