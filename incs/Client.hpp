/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:02:57 by yfontene          #+#    #+#             */
/*   Updated: 2025/02/14 19:41:34 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "utils.hpp"

class Client
{
    public:
        Client();
        Client(int fd, const std::string &host);
        ~Client();
        
        std::string buffer;
        
        int getFd() const;
        std::string getHost() const;
        std::string getNickname() const;
        std::string getUsername() const;
        const std::string &getRealname() const;
        const std::string &getModes() const;
        
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        void setRealname(const std::string &realname);
        void setMode(char mode, bool enable);
        void setPassAuthenticated(bool status);
        void setWelcomed(bool welcomed);

        //bool isAuthenticated() const;
        bool passAuthenticated() const;
        bool isFullyAuthenticated() const;
        bool isConnected() const;
        bool isWelcomed() const;
        bool hasMode(char mode) const;

        bool operator==(const Client &compare) const; //Used inside Channel::findUser()
    private:
        int _fd;
        std::string _host;
        bool _passAuthenticated;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _modes;
        bool _welcomed; // Flag para indicar se o cliente já recebeu a mensagem de boas-vindas
};

#endif
