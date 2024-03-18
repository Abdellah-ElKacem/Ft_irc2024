#include "Server.hpp"
// static struct sockaddr_in __serv_addr;
Server::Server(const std::string port, const std::string password) : _password(password)
{
    std::memset(&__serv_addr, 0, sizeof(__serv_addr));
    for (size_t i = 0; i < port.size(); i++) {
        if (!std::isdigit(port[i])) {
            std::cerr << "Error: Invalid port :/\n";
            exit(EXIT_FAILURE);
        }
    }
    _port = std::atol(port.c_str());
    _the_port = port;
    __serv_addr.sin_family = AF_INET;
    __serv_addr.sin_port = htons(_port);
    __serv_addr.sin_addr.s_addr = INADDR_ANY;
    init__and_run();
}

int Server::getPort() const {
    return _port;
}

struct pollfd add_to_poll(int _fd) {
    struct pollfd fd;
	fd.fd = _fd;
	fd.events = POLLIN;
	fd.revents = 0;
	return fd;
}

std::string Server::getPassword() const {
    return _password;
}

void Server::identify(int fd_client) {
    std::string message;
    message = "       _Welcome to the Server_\n\r";
    send(fd_client, message.c_str() , message.size(), 0);
    message = "           <~ ::Enter:: ~>\n<Password> - <Nick_name> - <Username>\n\r";
    send(fd_client, message.c_str() , message.size(), 0);
}

int Server::accept_func() {
    socklen_t addrlen = sizeof(struct sockaddr);
    int client_fd = accept(_server_sock,(struct sockaddr*)&__clients, (socklen_t*)&addrlen);
    if (client_fd == -1) {
        std::perror("Accept");
        return 1;
    }
    identify(client_fd);
	_fds.push_back(add_to_poll(client_fd));
    Clients client(client_fd);
    map_of_clients[client.GetNickname()] = client;
    std::cout << "client connected\n";
    return 0;
}

void Server::init__and_run()
{
    if ((_server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::perror("socket");
        exit(EXIT_FAILURE);
    }
    int ok = 1;
    if (setsockopt(_server_sock, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(int)) < 0) {
        std::perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }
    if (bind(_server_sock, (sockaddr*)&__serv_addr, sizeof(__serv_addr)) == -1) {
        std::perror("Error Bind");
        exit(EXIT_FAILURE);
    }
    if (listen(_server_sock,NUM_OF_CONNECTIONS) == -1) {
        std::perror("Error Listen");
        exit(EXIT_FAILURE);
    }

	_fds.push_back(add_to_poll(_server_sock));

    // non non blocking
    while(1)
    {
        int status = poll(_fds.data(),_fds.size(), -1);
        if (status == -1) {
            std::perror("poll");
            break;
        } for (size_t i = 0; i < _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN) {
                char buff[1024];
                std::memset(buff, 0, sizeof(buff));
                if (_fds[i].fd == _server_sock) {
                    if (accept_func())
                        continue;
                } else {
                    ssize_t recvv = recv(_fds[i].fd,buff, sizeof(buff) , 0);
                    if (recvv == -1) {
                        std::cout << "failed recv\n";
                    }
                    if (recvv == 0) {
                        std::cout << "client disconnected\n";
                        close(_fds[i].fd);
                        _fds.erase(_fds.begin() + i);
                    }
                    
                    if (recvv)
                        std::cout << "the client {" << _fds[i].fd << "} said : " << buff;
                }
            }
            if (_fds[i].revents & POLLERR) {
                std::cout<< "error:" << _fds[i].fd << '\n';
            }
        }
    }
    for (size_t i = 0; i < _fds.size(); i++)
    {
        close(_fds[i].fd);
        _fds.erase(_fds.begin() + i);
    }
}
