/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:02:57 by yfontene          #+#    #+#             */
/*   Updated: 2025/02/01 18:21:29 by arepsa           ###   ########.fr       */
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
    private:
        int _fd;
        std::string _nickname;
        std::string _username;
};

#endif