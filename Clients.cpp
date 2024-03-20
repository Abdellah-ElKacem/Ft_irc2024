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
    __newline_ = false;
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

void Clients::setNickname(std::string nick) {
    _nick_name = nick;
}

void Clients::setUsername(std::string user) {
    _user_name = user;
}

void Clients::SetBuffer(std::string buff)
{
    if (GetBoolNewline() == true) {
        _buffer_cl.clear();
        _buffer_cl.append(buff);
        SetBoolNewline(false);
        return ;
    }
    _buffer_cl.append(buff);
}

void Clients::SetBoolPassword(bool val) {
    __Password_ = val;
}

void Clients::SetBoolNickname(bool val) {
    __NickName_ = val;
}

void Clients::SetBoolUsername(bool val) {
    __UserName_ = val;
}

void Clients::SetBoolIdentify(bool val) {
    __identify_ = val;
}

void Clients::SetBoolNewline(bool val) {
    __newline_ = val;
}

bool Clients::GetBoolPassword() const
{
    return __Password_;
}

bool Clients::GetBoolNickname() const
{
    return __NickName_;
}

bool Clients::GetBoolUsername() const
{
    return __UserName_;
}

bool Clients::GetBoolIdentify() const
{
    return __identify_;
}

bool Clients::GetBoolNewline() const
{
    return __newline_;
}

std::string Clients::GetBuffer() const
{
    return _buffer_cl;
}
