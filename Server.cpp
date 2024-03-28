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

bool Server::parce_nick(std::string &part2) {
    if (std::isdigit(part2.c_str()[0]))
        return false;
    for (size_t i = 0; i < part2.length(); i++) {
        if ( !std::isalnum(part2[i]) && part2[i] != '[' && part2[i] != ']' && part2[i] != '{' \
            && part2[i] != '}' && part2[i] != '|' && part2[i] != '\\' )
                return false;
    }
    return true;    
}

int Server::switch_aft(std::string cmd) {
    if(!std::strcmp(cmd.c_str(), "JOIN"))
        return 0;
    if(!std::strcmp(cmd.c_str(), "KICK"))
        return 1;
    if(!std::strcmp(cmd.c_str(), "INVITE"))
        return 2;
    if(!std::strcmp(cmd.c_str(), "TOPIC"))
        return 3;
    if(!std::strcmp(cmd.c_str(), "MODE"))
        return 4;
    return 5;
}

void Server::if_authenticate_client(Clients& client) {
    std::string cmd, msg, cmd_p, part_cmd, old_nick;
    std::string the_command[6] = {"JOIN", "KICK", "INVITE", "TOPIC", "MODE", "PRVMSG"};
    std::string regis_cmd[2] = {"PASS", "USER"};
    cmd = client.GetBuffer().substr(0,client.GetBuffer().find(" "));
    cmd_p = cmd;
    for (size_t i = 0; i < cmd.size(); i++) {
        cmd[i] = std::toupper(cmd.c_str()[i]);
    } if ( cmd != "JOIN" && cmd != "KICK" && cmd != "INVITE" && cmd != "TOPIC" && cmd != "MODE" && cmd != "PRVMSG" && cmd != "PASS" && cmd != "USER" && cmd != "NICK" ){
        msg = ":ircserv_KAI.chat 421 " + client.GetNickname() + ' ' + cmd_p + " :Unknown command\r\n";
        msg_client(client.GetFdClient(), msg);
    } else if (cmd == "PASS" || cmd == "USER") {
        msg = ":ircserv_KAI.chat 462 " + client.GetNickname() + " :You may not reregister\r\n";
        msg_client(client.GetFdClient(), msg);
    } else if (cmd == "NICK") {
        old_nick = client.GetNickname();
        if (client.GetBuffer().find(" ") != client.GetBuffer().npos) {
            part_cmd = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
            if(!parce_nick(part_cmd)) {
                msg = ":ircserv_KAI.chat 432 " + client.GetNickname() + " NICK :Erroneus nickname\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            }
        }
        else
            part_cmd = "";
        if (part_cmd.empty()) {
            msg = ":ircserv_KAI.chat 431 " + client.GetNickname() + " NICK :No nickname given\r\n";
            msg_client(client.GetFdClient(),msg);
            return;
        }
        std::map<int, Clients>::iterator it;
        for ( it = map_of_clients.begin(); it != map_of_clients.end(); it++) {
            if (it->second.GetNickname() == part_cmd) {
                msg = ":ircserv_KAI.chat 433 " + part_cmd + " NICK :Nickname is already in use\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            }
        }
        //old_nick!~user_n@ip nick_cmd :new_nick
        client.setNickname(part_cmd);
        msg = old_nick + "!~" + client.GetUsername() + '@' + client.GetIpClient() + " NICK :" + client.GetNickname() + "\r\n";
        msg_client(client.GetFdClient(), msg);
    }
    return;
}
//inet_ntoa
void Server::register_client(Clients& client) {

    std::string part1, part2, part5 = "", msg;
    std::string after_regis[6] = {"JOIN", "KICK", "INVITE", "TOPIC", "MODE", "PRVMSG"};

    if (client.GetBoolOk() == false) {
        part1 = client.GetBuffer().substr(0,client.GetBuffer().find(" "));
        // std::cout <<"part1 -> :"<< part1 << std::endl;
        for (size_t i = 0; i < part1.size(); i++) {
            part1[i] = std::toupper(part1.c_str()[i]);
        } if (part1 == "JOIN" || part1 == "KICK" || part1 == "INVITE" || part1 == "TOPIC" || part1 == "MODE" || part1 == "PRVMSG") {
            int index = switch_aft(part1);
            msg = ":ircserv_KAI.chat 451 " + client.GetNickname() + ' ' + after_regis[index] + " :You have not registered\r\n";
            msg_client(client.GetFdClient(), msg);
            return;
        }
        if ( part1 == "PASS" && client.GetBoolPassword() == false ) {
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
            // if (part1 == "PASS" && client.GetBoolPassword() == true) {
            //         msg = ":ircserv_KAI.chat -1 " + client.GetNickname() + " PASS :You are already put the password\r\n";
            //     msg_client(client.GetFdClient(),msg);
            //     return;
            // }
            if ( part1 == "NICK" ) {
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
                std::cout << "the client register on the ircserv_KAI.chat server and his NickName is : < " << client.GetNickname() << " > and realname is : < " << client.GetRealname() << " >\n";
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
    message = "_Welcome to the \" ircserv_KAI.chat \" server_\r\n";
    send(fd_client, message.c_str() , message.size(), 0);
    message = "              <~ ::Enter:: ~>\n    <Password> - <Nick_name> - <Username>\r\n";
    send(fd_client, message.c_str() , message.size(), 0);
}

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
                    _buffer.clear();
                    _buffer.append(buff);
                    // _buffer.append("pass 1\nnick as\nuser asd sad sad asd asd \r\n");
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

