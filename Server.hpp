#pragma once

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <cstring>
#include <unistd.h>

class Server
{
    public:
        Server(const std::string port, const std::string password);
        ~Server() {};

        int getPort() const;
        std::string getPassword() const;

        void init_serv();
    private :
        std::vector<pollfd> _fds;
        int _server_sock;
        sockaddr_in _serv_addr;
        int _port;
        std::string _password;
};
