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
        std::vector<std::string> _members_list;
        std::vector<std::string> _invited_list;
        bool _is_invited;
        bool _is_topiced;

        // channel();
        // ~channel();
};

void check_cmd(std::map<int ,Clients>::iterator it);
