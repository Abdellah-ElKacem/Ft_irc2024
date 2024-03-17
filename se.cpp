#include <iostream>
#include <string>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 


int main() {
    // ... Socket setup (creation, binding, listening) ...

    // Accept a client connection
    int client_socket = accept(server_socket, NULL, NULL); 

    // Receive data from the client
    char buffer[1024] = {0};
    int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_read < 0) {
        perror("Error reading from socket");
    } else if (bytes_read == 0) {
        std::cout << "Client closed the connection" << std::endl;
    } else {
        std::cout << "Received: " << buffer << std::endl;

        // Process the data in the buffer ...

        // Send a response back to the client (optional)
        std::string response = "Data received and processed!"; 
        send(client_socket, response.c_str(), response.length(), 0); 
    }

    // Close the sockets
    close(client_socket);
    close(server_socket);

    return 0;
}
