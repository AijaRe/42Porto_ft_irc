/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 18:43:27 by yfontene          #+#    #+#             */
/*   Updated: 2025/01/31 19:07:20 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#include <sstream>
#include "utils.hpp"

class Server;

class CommandHandler {
    public:
        CommandHandler(Server &server);
        void executeCommand(int client_fd, const std::string &message);

    private:
        Server &_server;

        void handleNick(int client_fd, const std::string &params);
        void handleUser(int client_fd, const std::string &params);
        void handleJoin(int client_fd, const std::string &params);
        void handlePrivMsg(int client_fd, const std::string &params);
       
};

#endif

