#include "Server.hpp"

std::map<int, Clients>map_of_clients;

Server::Server(const std::string port, const std::string password)
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
    _password = password;
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

//----------------------> MSGRPLFORMAT [":<NCIKNAME>!~<REALNAME>@<HOSTNAME> <CMD> <TARGET> :<MSG>"]

// ERRORREPLYFORMAT [":<ServerName> <StatusCode> <CLientNick> :<Msg>"]


int Server::accept_func()
{
    std::string ip_client;
    socklen_t addrlen = sizeof(struct sockaddr);
    int client_fd = accept(_server_sock,(struct sockaddr*)&__clients, (socklen_t*)&addrlen);
    if (client_fd == -1) {
        if (errno != EWOULDBLOCK)
            std::perror("Accept");
        return 1;
    }
    welcome_client(client_fd);
    ip_client = inet_ntoa(__clients.sin_addr);
    if (!ip_client.c_str()) {
        std::cerr << "inet_ntoa failed.\n";
        return 0;
    }
	_fds.push_back(add_to_poll(client_fd));
    Clients client(client_fd, ip_client);
    map_of_clients[client.GetFdClient()] = client;
    std::cout << "a client connected\n";
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
    if (listen(_server_sock, NUM_OF_CONNECTIONS) == -1) {
        std::perror("Error Listen");
        exit(EXIT_FAILURE);
    }

	_fds.push_back(add_to_poll(_server_sock));

    std::map<int, Clients>::iterator it;

    // non non blocking
    fcntl(_server_sock, F_SETFL, O_NONBLOCK);
    while(1)
    {
        int status = poll(_fds.data(),_fds.size(), -1);
        if (status == -1) {
            std::perror("poll");
            break;
        } for (size_t i = 0; i < _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN) {
                if (_fds[i].fd == _server_sock) {
                    while (1) {
                        if (accept_func())
                            break;  
                    } continue;
                } else {
                    char buff[1024];
                    std::memset(buff, 0, sizeof(buff));
                    ssize_t recvv = recv(_fds[i].fd,buff, sizeof(buff) , 0);
                    // _buffer.clear();
                    // _buffer.append(buff);
                    _buffer = buff;
                    if (recvv == -1) {
                        std::cout << "failed recv\n";
                    }
                    if (recvv == 0) {
                        std::cout << "client disconnected\n";
                        close(_fds[i].fd);
                        _fds.erase(_fds.begin() + i);
                        continue;
                    }
                    if (recvv) {
                        it = map_of_clients.find(_fds[i].fd);
                        it->second.SetBoolOk(false);
                        it->second.SetBoolNewline(false);
                        it->second.SetBuffer_tmp(_buffer);
                        while (!it->second.GetBuffer_tmp().empty()) {
                            it->second.check_new_line();
                            it->second.trim_string();
                            // std::cout << "buff : [" << it->second.GetBuffer() << "]" << std::endl;
                            if (it->second.GetBoolNewline() == true) {
                                if (it->second.GetBoolIdentify() == false)
                                    register_client(it->second);
                                else
                                    if_authenticate_client(it->second);
                            }
                            if (it->second.GetBoolNewline() == true)
                                it->second.Buff_clear();
                        }
                    }
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

