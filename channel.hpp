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
        bool _is_invited;
        bool _is_topiced;
        bool _is_locked;
        channel();
};

// typdefinition
    typedef std::map<std::string, channel>::iterator channels;
    typedef std::map<int ,Clients>::iterator client;

void ft_handle_cmd(client& , std::vector<std::string> &);
void check_cmd(std::map<int ,Clients>::iterator it);
void pars_join_mode(std::vector<std::string> cmd);

extern std::map<std::string, channel> _channel_list;
extern std::map<std::string, Clients> _channel_list;
