#pragma once

#include <string>
#include <vector>
#include <map>
#include "Clients.hpp"
#include "replays.hpp"

// #define ERR_NEEDMOREPARAMS(hostname, nick, cmd) hostname + " 461 " + nick + " " + cmd + " :Not enough parameters\r\n"
// #define ERR_NOT_ON_CHANNEL(hostname, nick, _channel) hostname + " 442 " + " " + nick + " " _channel + " :You're not on that channel\r\n"
// #define ERR_NOT_OPERATOR(hostname, nick, _channel) hostname + " 482 " + nick + " " + _channel + " :You're not a channel operator\r\n"
// #define NO_SUCH_CHANNEL(nick, _channel) ":ircserv_KAI.chat" + " 403 " + nick + " " + _channel +  + " :No such channel\r\n"
// #define RPL_NO_TOPIC_SET(hostname, nick, _channel) hostname + " 331 " + nick + " " + _channel + " :No topic is set\r\n"
// #define RPL_TOPIC_VALUE(hostname, nick, _channel, topic) hostname + " 332 " + nick + " " + _channel + " :" + topic + "\r\n"
// #define RPL_TOPIC_SET(hostname, nick, _channel, topic, cmd) hostname + " 332 " + nick + " " + _channel + " :" + topic + "\r\n"



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

void send_rep(int fd, std::string msg);
void ft_handle_cmd(std::map<int ,Clients>::iterator& , std::vector<std::string> &);
void check_cmd(std::map<int ,Clients>::iterator it);
void pars_join_mode(std::vector<std::string> cmd, std::map<int ,Clients>::iterator it_c);
void creat_channel(std::map<std::string, channel>& _channel_list, std::map<int ,Clients>::iterator it_c, std::string name_ch);
void join_user_to_channel(std::map<int ,Clients>::iterator it_c, std::map<std::string, channel>::iterator it, std::vector<std::string> pass_wd, int i);
