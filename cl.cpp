#include <iostream>
#include <string>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 

int main() {
    int client_socket;
    struct sockaddr_in server_addr; 
    char buffer[1024] = {0};

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // ... Error handling ...

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000); // Match the port used by the server

    // Convert IPv4 address from text to binary form
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); 

    // Connect to the server
    connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // ... Error handling ....

    // Data to send to the server
    std::string data_to_send = "Hello from the client!";
    send(client_socket, data_to_send.c_str(), data_to_send.length(), 0);

    // Receive a response from the server (optional)
    int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    // ... Error handling ...
    std::cout << "Server response: " << buffer << std::endl;

    // Close the socket
    close(client_socket);

    return 0;
}
