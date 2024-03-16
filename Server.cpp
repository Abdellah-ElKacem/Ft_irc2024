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
	_fds.push_back(add_to_poll(_server_sock));
    if (listen(_server_sock,NUM_OF_CONNECTIONS) == -1) {
        std::perror("Error Listen");
        exit(EXIT_FAILURE);
    }
    while(1) {
        std::cout << "client connected\n";
        int status = poll(_fds.data(),_fds.size(), 4000);
        if (status == 0) {
            // std::cerr << "poll time out \n";
            continue;
        }
        for (size_t i = 0; i < _fds.size(); i++) {
            if (_fds[i].revents & POLLIN) {
                std::cout << _fds[i].fd << '\n';
            }
        }
        // else {
        //     for (size_t i = _fds[0].revents & POLLIN; i < _fds.size(); i++) {
        //         if (i) {
        //             std::cout << "word is : " << _fds[0].fd << '\n';
        //         } else {
        //             std::cout << "ocuup : " << _fds[0].revents << '\n';
        //         }
            
        // }
        // std::cout << status ;
    }
    // // std::cout << "Ready for client connect().\n";
    // int ac = accept(_server_sock,NULL,NULL);
    // if (ac == -1) {
    //     std::perror("Error Accept");
    // }
}
