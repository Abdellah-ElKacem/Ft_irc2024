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
#include <fcntl.h>
#include <sstream>
#include <ctime>
#include <fstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "Clients.hpp"
#include "channel.hpp"

class Clients;

#define NUM_OF_CONNECTIONS 10
class channel;

class Server
{
    public:
        Server(const std::string port, const std::string password);
        ~Server() {};

        int         getPort() const;
        std::string getPassword() const;

        void init__and_run();
    private :
        std::vector<pollfd> _fds;
        struct sockaddr_in  __serv_addr, __clients;
        int                 _server_sock;
        int                 _port;
        std::string         _password;
        std::string         _the_port;
        std::string         _buffer;

        int     accept_func();
        void    trim_string();
        void    delete_client_for_all(int);
        int     switch_aft(std::string cmd);
        int     for_iden_user(std::string&, std::string&);
        void    welcome_client(int fd_client);
        void    msg_client(int fd_client, std::string);
        void    if_authenticate_client(Clients&);
        void    register_client(Clients& client ,std::string &str_m ,std::string &str_d ,std::string &str_y ,std::string &str_h ,std::string &str_mi ,std::string &str_s);
        void    time_server(std::string &str_m ,std::string &str_d ,std::string &str_y ,std::string &str_h ,std::string &str_mi ,std::string &str_s);
        bool    parce_nick(std::string&);
};


extern std::map<int, Clients>map_of_clients;
extern std::map<std::string, Clients>nick_clients;

bool    has_only_space(std::string str);