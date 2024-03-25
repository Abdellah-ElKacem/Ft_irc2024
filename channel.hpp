#pragma once

#include <string>
#include <vector>
#include <map>
#include "Clients.hpp"


class channel
{
    public:

        std::string _ch_name;
        std::string _topic_name;
        std::string _pass;
        std::vector<std::string> _members_list;
        std::vector<std::string> _invited_list;
        std::vector<std::string> _operetos_list;
        unsigned long       _limit_nb;
        bool                _is_invited;
        bool                _is_topiced;
        bool                _is_locked;
        bool                _limit_members;

        channel();
        channel(std::string ch_name);
        ~channel();
};

// typdefinition
    typedef std::map<std::string, channel>::iterator channels;
    typedef std::map<int ,Clients>::iterator client;
    typedef std::vector<std::string>::iterator vector_it;


extern std::map<std::string, channel> _channel_list;


void ft_handle_cmd(std::map<int ,Clients>::iterator& , std::vector<std::string> &);
void check_cmd(std::map<int ,Clients>::iterator it);
void pars_join_mode(std::vector<std::string> cmd, std::map<int ,Clients>::iterator it_c);
void creat_channel(std::map<std::string, channel>& _channel_list, std::map<int ,Clients>::iterator it_c, std::string name_ch);
void join_user_to_channel(std::map<int ,Clients>::iterator it_c, std::map<std::string, channel>::iterator it, std::vector<std::string> pass_wd, int i);
