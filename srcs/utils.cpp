/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:32:34 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/23 19:39:23 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

void logMessage(const std::string &type, const std::string &message)
{
    if (type == "ERROR")
        std::cerr << "[" << type << "] " << message << std::endl;
    else
        std::cout << "[" << type << "] " << message << std::endl;
}

void sendError(int fd, const std::string& message)
{
    std::string errorMessage = "ERROR: " + message + "\r\n";
    std::cout << "Sending error to client " << fd << ": " << errorMessage << std::endl;
    ssize_t bytes_sent = send(fd, errorMessage.c_str(), errorMessage.length(), 0);
    if (bytes_sent == -1)
        std::cerr << "[ERROR] Failed to send error message to client " << fd << std::endl;
}

void sendReturn(int fd, const std::string& message)
{
    std::string returnMessage = message + "\r\n";
    const char* retMsg = returnMessage.c_str();
    send(fd, retMsg, returnMessage.length(), 0);
}

std::vector<std::string> splitString(const std::string &str, char delim)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delim);

    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delim, start);
    }
    result.push_back(str.substr(start, end)); //add last token after delimiter
    return result;
}

