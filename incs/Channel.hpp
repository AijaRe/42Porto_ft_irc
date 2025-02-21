/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:18 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/15 16:04:09 by yfontene         ###   ########.fr       */
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
        const Client* findUserByName(const std::map<std::string, const Client*> &user_map, const std::string &name) const;
        bool removeUser(std::map<std::string, const Client*> &user_map, std::string &user_to_remove);

        void broadcastMessage(std::map<std::string, const Client*> &user_map, const std::string &message);

        std::string getName();
        void setName(const std::string &new_name);

        std::string getChannelKey();

        std::string getCreator();
        
        int  getType();
        void setType(int new_type);

        std::string getTopic();
        void        setTopic(std::string &new_topic);

        void setMode(char mode, bool enable);
        void setModePass(const std::string &modepass);
        void setModeLimit(int limit);
        bool hasMode(char mode) const;
        std::string getModes() const;
        void sendToChannel(const std::string &message, const Server &server);

        std::map<std::string, const Client*> &getClients();
        std::map<std::string, const Client*> &getOps();
    private:
        Replies _replies;
    
        std::string _channel_key; //_type + _name
        std::string _name;
        std::string _topic;
        std::string _created_by;
        int         _type; //Check utils.hpp
        std::map<std::string, const Client *> _clients;
        std::map<std::string, const Client *> _ops;

        //para MODES
        std::string _modes;
        std::string _modepass;// para armazenar senha do canal(se modo +k for ativado)
        int _userlimit;//para armazenar limite de usuarios(se modo +l for ativado)
};

#endif
