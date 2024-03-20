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

void Server::register_client(Clients& client) {

    std::string part1, part2;

    part1 = client.GetBuffer().substr(0,client.GetBuffer().find(" "));
    for (size_t i = 0; i < part1.size(); i++) {
        part1[i] = std::toupper(part1.c_str()[i]);
    } if (part1 != "PASS" && part1 != "NICK" && part1 != "USER") {
        msg_client(client.GetFdClient(),"invalid Command\n");
        return;
    }
    // std::cout << std::boolalpha << client.GetBoolIdentify() << '\n';
    if ( client.GetBoolPassword() == false) {
        if(part1 != "PASS") {
            msg_client(client.GetFdClient(),"Set the Password first\n");
            return;
        }
        if (client.GetBuffer().find(" ") != client.GetBuffer().npos)
            part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
        else
            part2 = "";
        if (part2.empty()) {
            msg_client(client.GetFdClient(),"Write the Password\n");
            return;
        } if (part2 != _password) {
            msg_client(client.GetFdClient(),"Wrong password, Try again...\n");
            return;
        }
        client.SetBoolPassword(true);
        if (part2.empty()) {
            msg_client(client.GetFdClient(),"Write the Password\n");
            return;
        } if (part2 != _password) {
            msg_client(client.GetFdClient(),"Wrong password, Try again...\n");
            return;
        }
        client.SetBoolPassword(true);
        return;
    } if ( client.GetBoolPassword() == true && client.GetBoolNickname() == false ) {
        if(part1 != "NICK") {
            msg_client(client.GetFdClient(),"Set NickName second\n");
            return;
        }
        if (client.GetBuffer().find(" ") != client.GetBuffer().npos)
            part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
        else
            part2 = "";
        if (part2.empty()) {
            msg_client(client.GetFdClient(),"Write the Nickname\n");
            return;
        }
        client.setNickname(part2);
        client.SetBoolNickname(true);
        return;
    } if ( client.GetBoolPassword() == true && client.GetBoolNickname() == true && client.GetBoolUsername() == false ) {
        if(part1 != "USER") {
            msg_client(client.GetFdClient(),"Set UserName third\n");
            return;
        }
        if (client.GetBuffer().find(" ") != client.GetBuffer().npos)
            part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
        else
            part2 = "";
        if (part2.empty()) {
            msg_client(client.GetFdClient(),"Write the Username\n");
            return;
        }
        client.setUsername(part2);
        client.SetBoolUsername(true);
        std::cout << "OK!...\n";
        msg_client(client.GetFdClient(),"U are registed, enjoy...\n");
        client.SetBoolIdentify(true);
    }
}

void Server::msg_client(int fd_client, std::string message) {
    if (send(fd_client, message.c_str() , message.size(), 0) == -1)
        std::cerr << "sa\n";
}

void Server::welcome_client(int fd_client) {
    std::string message;
    message = "       _Welcome to the Server_\n\r";
    send(fd_client, message.c_str() , message.size(), 0);
    message = "           <~ ::Enter:: ~>\n<Password> - <Nick_name> - <Username>\n\r";
    send(fd_client, message.c_str() , message.size(), 0);
}

void Server::authenticate_client(Clients& client) {
        register_client(client);
}

int Server::accept_func()
{
    socklen_t addrlen = sizeof(struct sockaddr);
    int client_fd = accept(_server_sock,(struct sockaddr*)&__clients, (socklen_t*)&addrlen);
    if (client_fd == -1) {
        std::perror("Accept");
        return 1;
    }
    welcome_client(client_fd);
	_fds.push_back(add_to_poll(client_fd));
    Clients client(client_fd);
    map_of_clients[client.GetFdClient()] = client;
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

    std::map<int, Clients>::iterator it;

    // non non blocking
    while(1)
    {
        int status = poll(_fds.data(),_fds.size(), -1);
        if (status == -1) {
            std::perror("poll");
            break;
        } for (size_t i = 0; i < _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN) {
                if (_fds[i].fd == _server_sock) {
                    if (accept_func())
                        continue;
                } else {
                    char buff[1024];
                    std::memset(buff, 0, sizeof(buff));
                    ssize_t recvv = recv(_fds[i].fd,buff, sizeof(buff) , 0);
                    _buffer.clear();
                    _buffer.append(buff);
                    if (recvv == -1) {
                        std::cout << "failed recv\n";
                    }
                    if (recvv == 0) {
                        std::cout << "client disconnected\n";
                        close(_fds[i].fd);
                        _fds.erase(_fds.begin() + i);
                        continue;
                    }
                    // std::cout << _buffer << _buffer.size() << " - [" << _buffer.back()<<  "]\n";
                    if (_buffer.back() == '\n') {
                        _buffer.pop_back();
                        map_of_clients[_fds[i].fd].SetBoolNewline(true);
                    } if (_buffer.back() == '\r')
                        _buffer.pop_back();
                    // std::cout << _buffer << _buffer.size() << '\n';
                    if (recvv) {
                        for (it = map_of_clients.begin(); it != map_of_clients.end(); it++) {
                            if (it->second.GetFdClient() == _fds[i].fd) {
                                it->second.SetBuffer(_buffer);
                                // std::cout << std::boolalpha << it->second.GetBoolNewline() << '\n';
                                if (it->second.GetBoolIdentify() == false)
                                    authenticate_client(it->second);
                                else
                                    std::cout << "the client {" << _fds[i].fd << "} said : " << it->second.GetBuffer() << std::endl;
                            }
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













// int Server::the_commands(char *buff, int i)
// {
//     std::map<std::string, Clients>::iterator it;
//     ssize_t recvv = recv(_fds[i].fd,buff, sizeof(buff) , 0);
//     _buffer.clear();
//     _buffer.append(buff);
//     if (recvv == -1) {
//         std::cout << "failed recv\n";
//     }
//     if (recvv == 0) {
//         std::cout << "client disconnected\n";
//         close(_fds[i].fd);
//         _fds.erase(_fds.begin() + i);
//         return 1;
//     }
//     if (_buffer.back()-- == '\r') {
//         _buffer.pop_back();
//         _buffer.pop_back();
//     } else
//         _buffer.pop_back();
//     if (recvv) {
//         for (it = map_of_clients.begin(); it != map_of_clients.end(); it++) {
//             if (it->second.GetFdClient() == _fds[i].fd)
//                 authenticate_client(it->second);
//         }
//     }
//         // std::cout << "the client {" << _fds[i].fd << "} said : " << _buffer << std::endl;
//     return 0;
// }