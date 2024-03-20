#pragma once

#include "Server.hpp"

class Clients
{
    private:
        int fd_of_client;
        // int pass_in_client;
        std::string _nick_name;
        std::string _user_name;
        std::string _buffer_cl;

        bool __Password_;
        bool __NickName_;
        bool __UserName_;
        bool __identify_;

    public:
        Clients(){};
        Clients(int fd);
        ~Clients(){}
        std::string GetNickname() const;
        std::string GetUsername() const;
        int GetFdClient() const;

        void setNickname(std::string nick);
        void setUsername(std::string user);
        void SetBuffer(std::string buff);

        void SetBoolPassword(bool);
        void SetBoolNickname(bool);
        void SetBoolUsername(bool);
        void SetBoolIdentify(bool);

        bool GetBoolPassword() const;
        bool GetBoolNickname() const;
        bool GetBoolUsername() const;
        bool GetBoolIdentify() const;

        std::string GetBuffer() const;
};