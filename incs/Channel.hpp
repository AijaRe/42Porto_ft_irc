/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:18 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/27 16:36:21 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "utils.hpp"
#include "Client.hpp"
#include "Replies.hpp"

#include <set>

class Client;
class Server;

class Channel
{
    public:
        Channel();
        Channel(Client &client, const std::string &name, const int &type);
        ~Channel();

        void addClient(Client &client);
        void addOp(Client &op); //Channel operators
        const Client* findUserByName(const std::map<std::string, const Client*> &user_map, const std::string &name) const;
        bool removeUser(std::map<std::string, const Client*> &user_map, std::string &user_to_remove);

        void broadcastMessage(std::map<std::string, const Client*> &user_map, const std::string &message);

        std::string getName();
        void setName(const std::string &new_name);

        std::string getChannelKey();

        std::string getCreator();

        unsigned int  getUserLimit();
        
        int  getType();
        void setType(int new_type);

        bool setTopic(const std::string &new_topic, const Client *author);
        
        // MODES
        void setMode(char mode, bool enable);
        void setModePass(const std::string &modepass);
        void setModeLimit(int limit);
        bool hasMode(char mode) const;
        std::string getModes() const;
        std::string getModePass() const;

        std::string getTopic() const;
        const Client* getTopicAuthor() const;
        std::string getTopicTime() const;
        
        void addInvitedUser(const std::string &nickname);
        void removeInvitedUser(const std::string &nickname);
        bool isInvited(const std::string &nickname) const;
        void addOperator(const std::string &nickname);
        void removeOperator(const std::string &nickname);
        bool isOperator(const std::string &nickname) const;
        
        void sendToChannel(const std::string &message, const Server &server);

        void sendChannelTopic(Client &client);
        void sendUserList(Client &client);

        std::map<std::string, const Client*> &getClients();
        std::map<std::string, const Client*> &getOps();

        std::map<std::string, const Client*> &getInviteList();
    private:
        Replies _replies;
    
        std::string _channel_key; //_type + _name
        std::string _name;
        std::string _created_by;
        int         _type; //Check utils.hpp
        std::map<std::string, const Client *> _clients;
        std::map<std::string, const Client *> _ops;
        std::map<std::string, const Client *> _invite_list;

        // MODES
        std::string _modes;
        std::string _modepass;// to store channel password if +k is set
        unsigned int _userlimit;//to store the user limit if +l is set
        std::set<std::string> _invitedUsers; //to store the invited users if +i is set

        std::string _topic;
        const Client* _topic_author;
        std::time_t _topic_time;
};

#endif
