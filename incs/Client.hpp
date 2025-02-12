/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:02:57 by yfontene          #+#    #+#             */
/*   Updated: 2025/02/12 19:42:38 by arepsa           ###   ########.fr       */
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
        
        bool isAuthenticated() const;
        void setAuthenticated(bool status);
        bool isConnected() const;
        bool hasMode(char mode) const;

        bool operator==(const Client &compare) const; //Used inside Channel::findClient()
    private:
        int _fd;
        std::string _host;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _modes;
        bool _authenticated;
};

#endif
