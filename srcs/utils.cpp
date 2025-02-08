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
    const char* error = errorMessage.c_str();
    send(fd, error, errorMessage.length(), 0);
}
