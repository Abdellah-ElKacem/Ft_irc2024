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
        int the_commands(char *, int i);
        void welcome_client(int fd_client);
        void msg_client(int fd_client, std::string);
        void authenticate_client(Clients&);
        void register_client(Clients&);
};


extern std::map<int, Clients>map_of_clients;