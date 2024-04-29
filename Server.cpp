#include "Server.hpp"
#include "channel.hpp"

std::map<int, Clients>map_of_clients;
std::map<std::string, Clients>nick_clients;

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
    _bot_fd = -1;
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

int Server::accept_func()
{
    std::string ip_client;
    socklen_t addrlen = sizeof(struct sockaddr);
    int client_fd = accept(_server_sock,(struct sockaddr*)&__clients, (socklen_t*)&addrlen);
    // std::cout << "fd_client : " << client_fd << '\n';
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
    std::cout << "a client connected" << std::endl;
    return 0;
}

void Server::delete_client_for_all(int i) {
    std::string nick_;
    std::map<int, Clients>::iterator it;
    std::map<std::string, Clients>::iterator it1;
    std::map<std::string, int>::iterator key_t_print;
    std::map<std::string, channel>::iterator it2;
    std::vector<std::string>::iterator it_nick;

    it = map_of_clients.find(_fds[i].fd);
    if (it != map_of_clients.end()) {
        nick_ = it->second.GetNickname();
        map_of_clients.erase(it);
    }

    it1 = nick_clients.find(nick_);
    if (it1 != nick_clients.end())
        nick_clients.erase(it1);

    for (it2 = _channel_list.begin(); it2 != _channel_list.end(); it2++) {
        it_nick = std::find(it2->second._members_list.begin(), it2->second._members_list.end(), nick_);
        if (it_nick != it2->second._members_list.end()) {
            it2->second._members_list.erase(it_nick);
        }
        it_nick = std::find(it2->second._invited_list.begin(), it2->second._invited_list.end(), nick_);
        if (it_nick != it2->second._invited_list.end()) {
            it2->second._invited_list.erase(it_nick);
        }
        it_nick = std::find(it2->second._operetos_list.begin(), it2->second._operetos_list.end(), nick_);
        if (it_nick != it2->second._operetos_list.end()) {
            it2->second._operetos_list.erase(it_nick);
        }
        key_t_print = it2->second._members_list1.find(nick_);
        if (key_t_print != it2->second._members_list1.end()) {
            it2->second._members_list1.erase(key_t_print);
        }
    }
    close(_fds[i].fd);
    _fds.erase(_fds.begin() + i);

}

bool has_only_space(std::string str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (!std::isspace(str.c_str()[i]))
            return false;
    }
    return true;
}

void Server::init__and_run()
{
    int ok = 1;
    if ((_server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::perror("socket");
        exit(EXIT_FAILURE);
    } if (setsockopt(_server_sock, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(int)) < 0) {
        close (_server_sock);
        std::perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    } if (setsockopt(_server_sock, SOL_SOCKET, SO_NOSIGPIPE, &ok, sizeof(int)) < 0) {
        close (_server_sock);
        std::perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    } if (bind(_server_sock, (sockaddr*)&__serv_addr, sizeof(__serv_addr)) == -1) {
        close (_server_sock);
        std::perror("Error Bind");
        exit(EXIT_FAILURE);
    } if (listen(_server_sock, NUM_OF_CONNECTIONS) == -1) {
        close (_server_sock);
        std::perror("Error Listen");
        exit(EXIT_FAILURE);
    }

	_fds.push_back(add_to_poll(_server_sock));

    std::map<int, Clients>::iterator it;
    std::string str_y, str_m, str_d, str_h, str_mi, str_s;
    time_server(str_y, str_m, str_d, str_h, str_mi, str_s);

    // non blocking
    fcntl(_server_sock, F_SETFL, O_NONBLOCK);
    const size_t Max_size_buff = 512;
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
                    ssize_t recvv = recv(_fds[i].fd,buff, sizeof(buff) - 1 , 0);
                    _buffer = buff;
                    if (_buffer.size() > Max_size_buff) {
                        _buffer.clear();
                        it = map_of_clients.find(_fds[i].fd);
                        std::string msg = ":ircserv_KAI.chat 417 " + it->second.GetNickname() + " :Input line was too long\r\n";
                        msg_client(_fds[i].fd,msg);
                        continue;
                    } if (recvv == 0 || _buffer.substr(0, 4) == "QUIT") {
                        delete_client_for_all(i);
                        std::cout << "client disconnected\n";
                        continue;
                    } if (recvv == -1 || has_only_space(_buffer) == true) {
                        if (recvv == -1)
                            std::cout << "failed recv\n";
                        continue;
                    } if (recvv) {
                        it = map_of_clients.find(_fds[i].fd);
                        it->second.SetBoolOk(false);
                        it->second.SetBoolNewline(false);
                        it->second.SetBuffer_tmp(_buffer);
                        if (it->second.GetBuffer_tmp().size() > Max_size_buff) {
                            it->second.GetBuffer_tmp().clear();
                            break;
                        }
                        while (!it->second.GetBuffer_tmp().empty()) {
                            it->second.check_new_line();
                            it->second.trim_string();
                            // std::cout << '['<< it->second.GetBuffer()<< "] " << it->first << std::endl;
                            if (it->second.GetBoolNewline() == true) {
                                if (it->second.GetBoolIdentify() == false)
                                    register_client(it->second, str_y, str_m, str_d, str_h, str_mi, str_s);
                                else {
                                    if_authenticate_client(it->second, _channel_list);
                                    check_cmd(it);
                                }
                            }
                        if (it->second.GetBoolNewline() == true)
                            it->second.Buff_clear();
                        // std::cout  << std::boolalpha << it->second.GetBoolPassword()<< " | " << it->second.GetBoolIdentify()<< std::endl;
                        }
                    }
                }
            }
        }
    }
    for (size_t i = 0; i < _fds.size(); i++)
    {
        close(_fds[i].fd);
        _fds.erase(_fds.begin() + i);
    }
}

