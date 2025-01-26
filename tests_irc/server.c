#include <iostream>
#include <string.h> // strlen
#include <unistd.h> // read, write, close
#include <sys/socket.h> // socket, bind, listen, accept, send
#include <netinet/in.h> // sockaddr_in
#include <poll.h> // poll
#include <vector> // std::vector

/* 
** A simple test to create server conenction with client
** and show an example of communication
*/
void log_message(const std::string &type, const std::string &message)
{
    std::cout << "[" << type << "] " << message << std::endl;
}


int main() {
	int server_fd, client_fd;
	struct sockaddr_in serv_addr, client_addr; // structure for IPv4 addresses
	socklen_t addrlen = sizeof(client_addr);
	char buffer[1024] = {0};
	int port = 6667;
	const char *hello = "Hello from server";
	std::vector<pollfd> clients;

	// Create socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		log_message("ERROR", "Socket creation failed");
		return 1;
	}

	// Set the server address struct
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; // IPv4 family
	serv_addr.sin_addr.s_addr = INADDR_ANY; // Any IP address
	serv_addr.sin_port = htons(port); // convert port to network byte order

	// Reuse port after server restart (therwise port will be in TIME_WAIT state)
	// Without this option, the server will not be able to bind to the same port
	// unless you kill the process manually or wait for the TIME_WAIT state to expire
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT , &opt, sizeof(opt)) < 0) {
		log_message("ERROR", "setsockopt failed");
		close(server_fd);
		return 1;
	}

	// Bind the socket to the address
	if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
	// sockaddr is a generic structure for socket addresses (IPv4, IPv6, etc)
	// ..used in the bind, connect, and accept functions
		log_message("ERROR", "Bind failed");
		close(server_fd);
		return 1;
	}

	// Listen for incoming connections
	if (listen(server_fd, 3) < 0) { // 3 is the maximum number of pending connections
		log_message("ERROR", "Listen failed");
		close(server_fd);
		return 1;
	}

	//std::cout << "Server listening on port " << port << std::endl;
	log_message("INFO", "Server listening on port " + std::to_string(port));

/*
	// Accept the incoming connection
	// This is a blocking call
	// The program will wait here until a client connects
	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
	if (client_fd < 0) {
		log_message("ERROR", "Accept failed");
		close(server_fd);
		return 1;
	}

	std::cout << "Client connected" << std::endl;
	while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
           log_message("ERROR", "Client disconnected");
            break;
        }

        buffer[bytes_read] = '\0';
        std::cout << "Received: " << buffer;

        // Handle PING from the client
        if (strncmp(buffer, "PING", 4) == 0) {
            std::string response = "PONG " + std::string(buffer + 5); // dynamic response
            send(client_fd, response.c_str(), response.length(), 0);
            std::cout << "Sent: " << response << std::endl;
        } else if (strncmp(buffer, "NICK", 4) == 0 || strncmp(buffer, "USER", 4) == 0) {
            // if message contains "NICK" or "USER" Send a welcome message after the handshake
            const char *welcome = "Welcome to the We Read The Manual!\r\n"; // static response
            send(client_fd, welcome, strlen(welcome), 0);
            std::cout << "Sent: " << welcome;
        } else {
            // send a generic response
			const char *answer = "Got some unconventional stuff from you\r\n";
			send(client_fd, answer, strlen(answer), 0);
        }
    }

	close(client_fd);
	close(server_fd);
*/
	// pooling
	pollfd server_poll = {server_fd, POLLIN, 0};// POLLIN: There is data to read
	clients.push_back(server_poll);

	while(true)
	{
		// Wait for events on the sockets
		int activity = poll(clients.data(), clients.size(), -1); // -1: wait indefinitely
		if (activity < 0)
		{
			log_message("ERROR", "Poll failed");
			break;
		}

		// Check if the server socket has an event(new client)
		if (clients[0].revents & POLLIN)
		{
			client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
			if (client_fd < 0)
			{
				log_message("ERROR", "Accept failed");
				break;
			}
			// Add the new client to the list of clients
			pollfd client_poll = {client_fd, POLLIN, 0};
			clients.push_back(client_poll);
			log_message("INFO", "New client connected");
		}

		//check clients existente
		for (size_t i = 1; i < clients.size(); ++i)
		{
			if(clients[i].revents & POLLIN)
			{
				memset(buffer, 0, sizeof(buffer));
				int bytes_read = read(clients[i].fd, buffer, sizeof(buffer) - 1);
				if (bytes_read <= 0)
				{
					log_message("ERROR", "Client disconnected");
					close(clients[i].fd);
					clients.erase(clients.begin() + i);
					--i;
				}
				else
				{
					buffer[bytes_read] = '\0';
					log_message("RECEIVED", buffer);
					// Handle PING/PONG from the client
					if (strncmp(buffer, "PING", 4) == 0) {
            			std::string response = "PONG " + std::string(buffer + 5); // dynamic response
            			send(client_fd, response.c_str(), response.length(), 0);
            			std::cout << "Sent: " << response << std::endl;
       				} else if (strncmp(buffer, "NICK", 4) == 0 || strncmp(buffer, "USER", 4) == 0) {
            			// if message contains "NICK" or "USER" Send a welcome message after the handshake
            			const char *welcome = "Welcome to the We Read The Manual!\r\n"; // static response
            			send(client_fd, welcome, strlen(welcome), 0);
            			std::cout << "Sent: " << welcome;
        			} else {
            			// send a generic response
						const char *answer = "Got some unconventional stuff from you\r\n";
						send(client_fd, answer, strlen(answer), 0);
					}
				}
			}
		}
	}
	close(client_fd);
	close(server_fd);
	return 0;
}	


/* 
** To test run the program
** To test with netcat: open separate terminal and run "nc localhost 6667"
** send any string from netcat terminal and verify the response
**
** To test connection with HexChat
** Open HexChat, add new Network localhost/6667
** Send a message from HexChat, e.g., "/msg #channel Hello" and verify the response
** Test HexChat handshake send from Hexhcat: "/quote NICK mynickname" OR 
** "/quote USER myusername 0 * :Real Name"
*/