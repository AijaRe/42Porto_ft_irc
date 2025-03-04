/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:02:57 by yfontene          #+#    #+#             */
/*   Updated: 2025/03/04 15:12:21 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "utils.hpp"

class Channel;

class Client
{
    public:
        Client();
        Client(int fd, const std::string &host);
        ~Client();
        
        std::string buffer;
        std::map<std::string, Channel*> joinedChannels;
        
        int getFd() const;
        std::string getHost() const;
        std::string getNickname() const;
        std::string getUsername() const;
        const std::string &getRealname() const;
        std::string getModes() const;
        std::string getTmpPass() const;
        
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        void setRealname(const std::string &realname);
        void setMode(char mode, bool enable);
        void setPassAuthenticated(bool status);
        void setWelcomed(bool welcomed);
        void setTmpPass(const std::string &new_pass);

        //bool isAuthenticated() const;
        bool passAuthenticated() const;
        bool isFullyAuthenticated() const;
        bool isConnected() const;
        bool isWelcomed() const;
        bool hasMode(char mode) const;

        void addChannel(Channel *channel);
        void removeChannel(Channel *channel);

        bool operator==(const Client &compare) const; //Used inside Channel::findUser()
    private:
        int _fd;
        std::string _host;
        bool _pass_authenticated;
        std::string _tmp_pass;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _modes;
        bool _welcomed; // Flag para indicar se o cliente já recebeu a mensagem de boas-vindas
};

#endif
