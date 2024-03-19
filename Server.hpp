#pragma once

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <map>

#include "Clients.hpp"

class Clients;

#define NUM_OF_CONNECTIONS 10

class Server
{
    public:
        Server(const std::string port, const std::string password);
        ~Server() {};

        int getPort() const;
        std::string getPassword() const;

        void init__and_run();
    private :
        std::vector<pollfd> _fds;
        struct sockaddr_in __serv_addr, __clients;
        int _server_sock;
        int _port;
        std::string _password;
        std::string _the_port;
        std::string _buffer;
        int accept_func();
        void identify(int fd_client);
};


extern std::map<std::string, Clients>map_of_clients;