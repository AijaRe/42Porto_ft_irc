/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 18:43:33 by yfontene          #+#    #+#             */
/*   Updated: 2025/01/31 18:43:34 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <map>

class User {
public:
    User();
    void setNick(const std::string &nick);
    void setUsername(const std::string &username);
    
    std::string getNick() const;
    std::string getUsername() const;
    bool isRegistered() const;
    std::map<int, User> _users;

private:
    std::string _nick;
    std::string _username;
};

#endif
