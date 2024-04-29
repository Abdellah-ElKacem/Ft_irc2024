#include "../Server.hpp"
#include "../channel.hpp"

void Server::if_authenticate_client(Clients& client, std::map<std::string, channel>& _channel_list) {
    
    std::string cmd, cmd_bf = client.GetBuffer(), msg, part_cmd, old_nick;
    cmd = client.GetBuffer().substr(0,client.GetBuffer().find(" "));
    std::map<int, Clients>::iterator it;
    
    if ( cmd.empty() )
        return;
    for (size_t i = 0; i < cmd.size(); i++) {
        cmd[i] = std::toupper(cmd.c_str()[i]);
    } for (size_t i = 0; i < cmd_bf.size(); i++) {
        cmd_bf[i] = std::toupper(cmd_bf.c_str()[i]);
    }
    if ( cmd != "JOIN" && cmd != "KICK" && cmd != "INVITE" && cmd != "TOPIC" && cmd != "MODE" && cmd != "PRIVMSG" \
            && cmd != "PASS" && cmd != "USER" && cmd != "NICK" && cmd != "PING" && cmd != "PONG" && cmd_bf != "/BOT") {
        msg = ":ircserv_KAI.chat 421 " + client.GetNickname() + ' ' + cmd + " :Unknown command\r\n";
        msg_client(client.GetFdClient(), msg);
    } else if (cmd == "PASS" || cmd == "USER") {
        msg = ":ircserv_KAI.chat 462 " + client.GetNickname() + " :You may not reregister\r\n";
        msg_client(client.GetFdClient(), msg);
    }
    else if (cmd == "NICK")
    {
        old_nick = client.GetNickname();
        if (client.GetBuffer().find(" ") != client.GetBuffer().npos) {
            part_cmd = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
            if (part_cmd.back() == ':')
                part_cmd.pop_back();
            if (part_cmd.back() == '_')
                return;
            if(!parce_nick(part_cmd)) {
                msg = ":ircserv_KAI.chat 432 " + client.GetNickname() + " NICK :Erroneus nickname\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            }
        }
        else
            part_cmd = "";
        if (part_cmd.empty())
        {
            msg = ":ircserv_KAI.chat 431 " + client.GetNickname() + " NICK :No nickname given\r\n";
            msg_client(client.GetFdClient(),msg);
            return;
        }
        for ( it = map_of_clients.begin(); it != map_of_clients.end(); it++ ) {
            if (it->second.GetNickname() == part_cmd) {
                msg = ":ircserv_KAI.chat 433 " + part_cmd + " NICK :Nickname is already in use\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            }
        }
        std::map<std::string, channel>::iterator it_ch;
        std::vector<std::string>::iterator it_nick;
        std::map<std::string, Clients>::iterator it_cl;

        std::map<std::string, int> add_to_print;
        std::map<std::string, int>::iterator add_to_print1, key_t_print;

        client.setNickname(part_cmd);
        it_cl = nick_clients.find(old_nick);
        if (it_cl != nick_clients.end()) {
            nick_clients.erase(it_cl);
            nick_clients.insert(std::make_pair(part_cmd, client));
        }
        add_to_print.insert(std::make_pair(client.GetNickname(), client.GetFdClient()));


        for (it_ch = _channel_list.begin(); it_ch != _channel_list.end(); it_ch++) {
            it_nick = std::find(it_ch->second._members_list.begin(), it_ch->second._members_list.end(), old_nick);
            if (it_nick != it_ch->second._members_list.end()) {
                it_ch->second._members_list.erase(it_nick);
                it_ch->second._members_list.push_back(client.GetNickname());
            }
            it_nick = std::find(it_ch->second._invited_list.begin(), it_ch->second._invited_list.end(), old_nick);
            if (it_nick != it_ch->second._invited_list.end()) {
                it_ch->second._invited_list.erase(it_nick);
                it_ch->second._invited_list.push_back(client.GetNickname());
            }
            it_nick = std::find(it_ch->second._operetos_list.begin(), it_ch->second._operetos_list.end(), old_nick);
            if (it_nick != it_ch->second._operetos_list.end()) {
                it_ch->second._operetos_list.erase(it_nick);
                it_ch->second._operetos_list.push_back(client.GetNickname());
            }
            key_t_print = it_ch->second._members_list1.find(old_nick);
            if (key_t_print != it_ch->second._members_list1.end()) {
                int fd = key_t_print->second;
                it_ch->second._members_list1.erase(key_t_print);
                it_ch->second._members_list1[client.GetNickname()] = fd;
            }

            it_nick = std::find(it_ch->second._members_list.begin(), it_ch->second._members_list.end(), client.GetNickname());
            if (it_nick != it_ch->second._members_list.end()) {
                for (key_t_print = it_ch->second._members_list1.begin(); key_t_print != it_ch->second._members_list1.end(); key_t_print++)
                    add_to_print[key_t_print->first] = key_t_print->second;
            }
        }
        msg = ":" + old_nick + "!~" + client.GetUsername() + '@' + client.GetIpClient() + " NICK :" + client.GetNickname() + "\r\n";
        for (add_to_print1 = add_to_print.begin(); add_to_print1 != add_to_print.end(); add_to_print1++) {
            msg_client(add_to_print1->second, msg);
        }
    }
    return;
}
