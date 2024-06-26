#include "Clients.hpp"

Clients::Clients(int fd, std::string ip_client) {
    _nick_name = "*";
    _user_name = "";
    _real_name = "";
    _buffer_cl = "";
    _fd_of_client = fd;
    _ip_client = ip_client;

    __Password_ = false;
    __NickName_ = false;
    __UserName_ = false;
    __identify_ = false;
    __newline_ = false;
    __he_know_ = false;
}

std::string Clients::GetNickname() const {
    return _nick_name;
}

std::string Clients::GetUsername() const {
    return _user_name;
}

std::string Clients::GetRealname() const {
    return _real_name;
}

int Clients::GetFdClient() const {
    return _fd_of_client;
}

std::string Clients::GetIpClient() const {
    return _ip_client;
}

void Clients::setNickname(std::string nick) {
    _nick_name = nick;
}

void Clients::setUsername(std::string user) {
    _user_name = user;
}

void Clients::setRealname(std::string real) {
    _real_name = real;
}

void Clients::Buff_clear() {
    _buffer_cl_final.clear();
}

void Clients::SetBuffer(std::string buff)
{
    if (GetBoolNewline() == true) {
        _buffer_cl.append(buff);
        _buffer_cl_final = _buffer_cl;
        _buffer_cl.clear();
        SetBoolNewline(false);
        SetBoolOk(true);
        return ;
    }
    _buffer_cl.append(buff);
}

void Clients::SetBuffer_tmp(std::string buff) {
    _buffer_cl = buff;
}

void Clients::SetBoolNewline(bool val) {
    __newline_ = val;
}

bool Clients::GetBoolNewline() const {
    return __newline_;
}

bool Clients::GetBoolOk() const {
    return __buff_ok_;
}

bool Clients::GetBoolKnow() const {
    return __he_know_;
}

int Clients::check_new_line() {

    size_t idx = _buffer_cl.find('\n');

    if (idx != _buffer_cl.npos) {
        _buffer_cl_final += _buffer_cl.substr(0, idx);
        if (_buffer_cl_final.size() > 512) {
            std::string msg = ":ircserv_KAI.chat 417 " + GetNickname() + " :Input line was too long\r\n";
            send_rep(GetFdClient(), msg);
            return 1;
        }
        _buffer_cl = _buffer_cl.substr(idx + 1);
        SetBoolNewline(true);
    } else {
        _buffer_cl_final.append(_buffer_cl);
        _buffer_cl.clear();
        SetBoolNewline(false);
        if (_buffer_cl_final.size() > 512) {
            std::string msg = ":ircserv_KAI.chat 417 " + GetNickname() + " :Input line was too long\r\n";
            send_rep(GetFdClient(), msg);
            return 1;
        }
    }
    if (_buffer_cl_final.at(_buffer_cl_final.length() - 1) == '\r')
        _buffer_cl_final.erase(_buffer_cl_final.begin() + _buffer_cl_final.size() - 1);
    return 0;
}

void Clients::trim_string() {
    if(!_buffer_cl_final.empty()) {
        size_t indx = _buffer_cl_final.find(":");
        std::string no_space, tmp, after_space;
        if (indx != _buffer_cl_final.npos) {
            after_space = _buffer_cl_final.substr(indx);
            _buffer_cl_final = _buffer_cl_final.substr(0, _buffer_cl_final.find(":"));
        } else
            after_space = "";
        std::stringstream trim(_buffer_cl_final);
        while (trim >> no_space)
            tmp.append(no_space + ' ');
        tmp.erase(tmp.size() - 1);
        if (after_space.empty())
            _buffer_cl_final = tmp + after_space;
        else
            _buffer_cl_final = tmp + ' ' + after_space;
    }
}

void Clients::SetBoolOk(bool val) {
    __buff_ok_ = val;
}

void Clients::SetBoolKnow(bool val) {
    __he_know_ = val;
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

std::string Clients::GetBuffer() const
{
    return _buffer_cl_final;
}

std::string Clients::GetBuffer_tmp() const
{
    return _buffer_cl;
}
