/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 18:44:04 by yfontene          #+#    #+#             */
/*   Updated: 2025/01/31 18:44:05 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/User.hpp"

User::User() : _nick(""), _username("") {}

void User::setNick(const std::string &nick) {
    _nick = nick;
}

void User::setUsername(const std::string &username) {
    _username = username;
}

std::string User::getNick() const {
    return _nick;
}

std::string User::getUsername() const {
    return _username;
}

bool User::isRegistered() const {
    return !_nick.empty() && !_username.empty();
}
