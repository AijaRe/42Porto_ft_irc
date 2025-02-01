/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:02:57 by yfontene          #+#    #+#             */
/*   Updated: 2025/02/01 19:26:36 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "utils.hpp"

class Client
{
    public:
        Client(int fd);
        ~Client();
        
        std::string buffer;
        
        int getFd() const;
        std::string getNickname() const;
        std::string getUsername() const;
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        
        bool isAuthenticated() const;
        void setAuthenticated(bool status);
    private:
        int _fd;
        std::string _nickname;
        std::string _username;
        bool _authenticated;
};

#endif