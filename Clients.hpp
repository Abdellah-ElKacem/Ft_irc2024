#pragma once

#include "Server.hpp"

class Clients
{
    private:
        int _fd_of_client;
        std::string _ip_client;
        std::string _nick_name;
        std::string _user_name;
        std::string _real_name;
        std::string _buffer_cl;
        std::string _buffer_cl_final;

        bool __Password_;
        bool __NickName_;
        bool __UserName_;
        bool __identify_;
        bool __newline_;
        bool __buff_ok_;

    public:
        Clients(){}
        Clients(int fd, std::string ip_client);
        ~Clients(){}
        std::string GetNickname() const;
        std::string& GetNicknameref() ;
        std::string GetUsername() const;
        std::string GetRealname() const;
        std::string GetBuffer() const;
        std::string GetBuffer_tmp() const;
        int GetFdClient() const;
        std::string GetIpClient() const;

        void setNickname(std::string nick);
        void setUsername(std::string user);
        void setRealname(std::string real);
        void Buff_clear();
        void SetBuffer(std::string buff);
        void SetBuffer_tmp(std::string buff);

        void SetBoolPassword(bool);
        void SetBoolNickname(bool);
        void SetBoolUsername(bool);
        void SetBoolIdentify(bool);
        void SetBoolNewline(bool);
        void SetBoolOk(bool);

        bool GetBoolPassword() const;
        bool GetBoolNickname() const;
        bool GetBoolUsername() const;
        bool GetBoolIdentify() const;
        bool GetBoolNewline() const;
        bool GetBoolOk() const;

        void check_new_line();
        void trim_string();

};
