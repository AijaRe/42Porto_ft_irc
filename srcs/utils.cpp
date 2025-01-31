#include "utils.hpp"

void logMessage(const std::string &type, const std::string &message)
{
    if (type == "ERROR")
        std::cerr << "[" << type << "] " << message << std::endl;
    else
        std::cout << "[" << type << "] " << message << std::endl;
}

void sendMessage(int client_fd, const std::string &message) {
    std::string formattedMessage = message + "\r\n";
    send(client_fd, formattedMessage.c_str(), formattedMessage.length(), 0);
}
