/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 20:08:43 by arepsa            #+#    #+#             */
/*   Updated: 2025/02/01 17:40:09 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <exception>
#include <string.h> // strlen
#include <unistd.h> // read, write, close
#include <vector> // std::vector
#include <map>
#include <sstream>
#include <sys/socket.h> // socket, bind, listen, accept, send
#include <netinet/in.h> // sockaddr_in
#include <poll.h> // poll
#include <cstdlib> // strtol
#include <cerrno> // errno
#include <signal.h>
#include <algorithm> //std::find()
#include <fcntl.h>

#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "CommandHandler.hpp"


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define UNDERLINE "\033[4m"

void logMessage(const std::string &type, const std::string &message);
void sendError(int fd, const std::string& message);

#endif