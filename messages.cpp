#include "Server.hpp"

void Server::msg_client(int fd_client, std::string message) {
    if (send(fd_client, message.c_str() , message.size(), 0) == -1)
        std::cerr << "Error: send\n";
}

void Server::welcome_client(int fd_client) {
    std::string message;
    message = "_Welcome to the \" ircserv_KAI.chat \" server_\r\n";
    send(fd_client, message.c_str() , message.size(), 0);
    message = "              <~ ::Enter:: ~>\n    <Password> - <Nick_name> - <Username>\r\n";
    send(fd_client, message.c_str() , message.size(), 0);
}