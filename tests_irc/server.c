#include <iostream>
#include <string.h> // strlen
#include <unistd.h> // read, write

#include <sys/socket.h> // socket, bidn, listen, accept
#include <netinet/in.h> // sockaddr_in

/* 
** A simple test to create server conenction with client
** and show an example of communication
*/

int main() {
	int server_fd, client_fd;
	struct sockaddr_in serv_addr, client_addr; // structure for IPv4 addresses
	socklen_t addrlen = sizeof(client_addr);
	char buffer[1024] = {0};
	int port = 6667;
	const char *hello = "Hello from server";

	// Creating socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		std::cerr << "socket failed" << std::endl;
		return 1;
	}

	// Set the server address struct
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; // IPv4 family
	serv_addr.sin_addr.s_addr = INADDR_ANY; // Any IP address
	serv_addr.sin_port = htons(port); // convert port to network byte order

	// Bind the socket to the address
	if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
	// sockaddr is a generic structure for socket addresses (IPv4, IPv6, etc)
	// ..used in the bind, connect, and accept functions
		std::cerr << "bind failed" << std::endl;
		close(server_fd);
		return 1;
	}

	// Listen for incoming connections
	if (listen(server_fd, 3) < 0) { // 3 is the maximum number of pending connections
		std::cerr << "listen failed" << std::endl;
		close(server_fd);
		return 1;
	}

	std::cout << "Server listening on port " << port << std::endl;

	// Accept the incoming connection
	// This is a blocking call
	// The program will wait here until a client connects
	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
	if (client_fd < 0) {
		std::cerr << "accept failed" << std::endl;
		close(server_fd);
		return 1;
	}

	std::cout << "Client connected" << std::endl;

	// Read the message from the client
	int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1); // -1 to leave space for the null terminator
	if (bytes_read < 0) {
		std::cerr << "read failed" << std::endl;
		close(server_fd);
		close(client_fd); 
		return 1;
	}

	buffer[bytes_read] = '\0'; // Null terminate the string

	std::cout << "Client says: " << buffer << std::endl;

	close(client_fd);
	close(server_fd);

}	

// To test run the program
// in separate terminal run "nc localhost 6667"

