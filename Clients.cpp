#include "Clients.hpp"

Clients::Clients(int fd) {
    _nick_name = "";
    _user_name = "";
    // pass_in_client = 0;
    fd_of_client = fd;

    __Password_ = false;
    __NickName_ = false;
    __UserName_ = false;
    __identify_ = false;
}

std::string Clients::GetNickname() const {
    return _nick_name;
}

std::string Clients::GetUsername() const {
    return _user_name;
}

int Clients::GetFdClient() const {
    return fd_of_client;
}
