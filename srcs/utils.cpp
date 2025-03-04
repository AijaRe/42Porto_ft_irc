/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 19:32:34 by uviana-a          #+#    #+#             */
/*   Updated: 2025/03/04 11:13:23 by arepsa           ###   ########.fr       */
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

std::string trimWhitespace(const std::string &str)
{
    size_t start = 0;
    size_t end = str.length() - 1;
    
    while (start < str.length() && std::isspace(str[start])) {
        start++;
    }
    while (end > start && std::isspace(str[end])) {
        end--;
    }
    if (end < start) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

void sendDirectMessage(int fd, const std::string &message) {
    std::stringstream ss;
    ss << "SEND TO FD: " << fd;  // Convert fd to string
    logMessage(ss.str(), message);
    
    //add /r/n to the message and send it to the client
    std::string messageToClient = message + "\r\n";
    send(fd, messageToClient.c_str(), messageToClient.length(), 0);
}


