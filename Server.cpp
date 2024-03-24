#include "Server.hpp"
#include "channel.hpp"

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

bool Server::parce_nick(std::string &part2) {
    for (size_t i = 0; i < part2.length(); i++) {
        if ( !std::isalnum(part2[i]) && part2[i] != '[' && part2[i] != ']' && part2[i] != '{' \
            && part2[i] != '}' && part2[i] != '|' && part2[i] != '\\')
                return false;
    }
    return true;    
}

void Server::register_client(Clients& client) {

    std::string part1, part2, part3 = "", part4 = "", part5 = "", msg;

    if (client.GetBoolNewline() == false && client.GetBoolOk() == true) {
        part1 = client.GetBuffer().substr(0,client.GetBuffer().find(" "));
        for (size_t i = 0; i < part1.size(); i++) {
            part1[i] = std::toupper(part1.c_str()[i]);
        } if (part1 != "PASS" && part1 != "NICK" && part1 != "USER") {
            msg_client(client.GetFdClient(),"invalid Command\r\n");
            return;
        }
        if ( client.GetBoolPassword() == false ) {
            if(part1 != "PASS") {
                msg_client(client.GetFdClient(),"Set the Password first\r\n"); // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                return;
            }
            if (client.GetBuffer().find(" ") != client.GetBuffer().npos)
                part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
            else
                part2 = "";
            if (part2.empty()) {
                msg = ":ircserv_KAI.chat 461 " + client.GetNickname() + " PASS :Not enough parameters\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            } if (part2 != _password) {
                msg = ":ircserv_KAI.chat 464 " + client.GetNickname() + " PASS :Password incorrect\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            }
            client.SetBoolPassword(true);
            return;
        } else if ( client.GetBoolPassword() == true ) {
            if (part1 == "PASS" && client.GetBoolPassword() == true) {
                    msg = ":ircserv_KAI.chat 462 " + client.GetNickname() + " PASS :You may not reregister\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            } if ( part1 == "NICK" ) {
                if (client.GetBuffer().find(" ") != client.GetBuffer().npos) {
                    part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
                    if(!parce_nick(part2)) {
                        msg = ":ircserv_KAI.chat 432 " + client.GetNickname() + " NICK :Erroneus nickname\r\n";
                        msg_client(client.GetFdClient(),msg);
                        return;
                    }
                }
                else
                    part2 = "";
                if (part2.empty()) {
                    msg = ":ircserv_KAI.chat 431 " + client.GetNickname() + " NICK :No nickname given\r\n";
                    msg_client(client.GetFdClient(),msg);
                    return;
                }
                std::map<int, Clients>::iterator it;
                for ( it = map_of_clients.begin(); it != map_of_clients.end(); it++) {
                    if (it->second.GetNickname() == part2) {
                        msg = ":ircserv_KAI.chat 433 " + part2 + " NICK :Nickname is already in use\r\n";
                        msg_client(client.GetFdClient(),msg);
                        return;
                    }
                }
                client.setNickname(part2);
                client.SetBoolNickname(true);
            } else if ( part1 == "USER" ) {
                // if ( client.GetBoolUsername() == true ) {
                //     msg = ":ircserv_KAI.chat 462 " + client.GetNickname() + " PASS :You may not reregister\r\n";
                //     msg_client(client.GetFdClient(),msg);
                //     return;
                // }
                if (client.GetBuffer().find(" ") != client.GetBuffer().npos) {
                    part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
                    if (for_iden_user(part2, part5) == 1){
                        msg = ":ircserv_KAI.chat 461 " + client.GetNickname() + " USER :Not enough parameters\r\n";
                        msg_client(client.GetFdClient(),msg);
                        return;
                    }
                } else
                    part2 = "";
                if (part2.empty()) {
                    msg = ":ircserv_KAI.chat 461 " + client.GetNickname() + " USER :Not enough parameters\r\n";
                    msg_client(client.GetFdClient(),msg);
                    return;
                }
                client.setUsername(part2);
                client.setRealname(part5);
                if (client.GetRealname().empty()) {
                    msg = ":ircserv_KAI.chat 461 " + client.GetNickname() + " USER :Not enough parameters\r\n";
                    msg_client(client.GetFdClient(),msg);
                    return;
                }
                client.SetBoolUsername(true);
            }
            if (client.GetBoolPassword() == true && client.GetBoolNickname() == true && client.GetBoolUsername() == true) {
                std::cout << "OK!...\n";
                std::cout << "the client register on the server and his NickName is : < " << client.GetNickname() << " > and realname is : < " << client.GetRealname() << " >\n";
                msg_client(client.GetFdClient(),"U are registed, enjoy...\n");
                client.SetBoolIdentify(true);
            }
        }
    }
}

int Server::for_iden_user(std::string &part2, std::string &part5) {
    std::string tmp;
    std::vector <std::string> identi_user;
    size_t idx = part2.find(' ');
    int idx_prv = 0;
    while(idx != part2.npos) {
        tmp = part2.substr(idx_prv, idx - idx_prv);
        identi_user.push_back(tmp);
        idx_prv = idx + 1;
        if (part2[idx_prv] == ':') {
            idx_prv++;
            break;
        }
        idx = part2.find(' ',idx_prv + 1);
        tmp.clear();
    }
    identi_user.push_back(part2.substr(idx_prv));
    if (identi_user.size() < 4)
        return 1;
    part2 = identi_user[0];
    part5 = identi_user[3];
    return 0;
}

void Server::msg_client(int fd_client, std::string message) {
    if (send(fd_client, message.c_str() , message.size(), 0) == -1)
        std::cerr << "Error: send\n";
}

void Server::welcome_client(int fd_client) {
    std::string message;
    message = "       _Welcome to the Server_\r\n";
    send(fd_client, message.c_str() , message.size(), 0);
    message = "           <~ ::Enter:: ~>\n<Password> - <Nick_name> - <Username>\r\n";
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
        if (errno != EWOULDBLOCK)
            std::perror("Accept");
        return 1;
    }
    welcome_client(client_fd);
	_fds.push_back(add_to_poll(client_fd));
    Clients client(client_fd);
    map_of_clients[client.GetFdClient()] = client;
    std::cout << "a client connected\n";
    return 0;
}

#include <fcntl.h>
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
                    std::cout << "server fd has incommimg connection(s)\n";
                    while (1)
                    {
                        if (accept_func())
                            break;
                        
                    }
                    // if (accept_func())
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
                    if (_buffer.back() == '\n') {
                        _buffer.pop_back();
                        map_of_clients[_fds[i].fd].SetBoolNewline(true);
                    } if (_buffer.back() == '\r')
                        _buffer.pop_back();
                    if (recvv) {
                        for (it = map_of_clients.begin(); it != map_of_clients.end(); it++) {
                            if (it->second.GetFdClient() == _fds[i].fd) {
                                it->second.SetBoolOk(false);
                                it->second.SetBuffer(_buffer);
                                if (it->second.GetBoolIdentify() == false)
                                    authenticate_client(it->second);
                                else
                                    check_cmd(it);
                                if (it->second.GetBoolNewline() == false)
                                    it->second.Buff_clear();
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
