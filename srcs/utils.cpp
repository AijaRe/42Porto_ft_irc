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

