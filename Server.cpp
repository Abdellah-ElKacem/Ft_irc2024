#include "Server.hpp"

Server::Server(const std::string port, const std::string password) : _password(password)
{
    for (size_t i = 0; i < port.size(); i++) {
        if (!std::isdigit(port[i])) {
            std::cerr << "Error: Invalid port :/\n";
            exit(EXIT_FAILURE);
        }
    }
    _port = std::atol(port.c_str());
    _serv_addr.sin_family = AF_INET;
}

int Server::getPort() const {
    return _port;
}

std::string Server::getPassword() const {
    return _password;
}



void Server::init_serv()
{
    if ((_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) == -1)) {
        std::cerr << "Error: socket failed !\n";
        close(server_sock);
        exit (EXIT_FAILURE);
    }

}
