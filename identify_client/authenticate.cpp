#include "../Server.hpp"

void Server::if_authenticate_client(Clients& client) {
    std::string cmd, msg, cmd_p, part_cmd, old_nick;
    cmd = client.GetBuffer().substr(0,client.GetBuffer().find(" "));
    
    if ( cmd.empty() || cmd == "PONG" || cmd == "QUIT" )
        return;
    cmd_p = cmd;
    for (size_t i = 0; i < cmd.size(); i++) {
        cmd[i] = std::toupper(cmd.c_str()[i]);
    } if ( cmd != "JOIN" && cmd != "KICK" && cmd != "INVITE" && cmd != "TOPIC" && cmd != "MODE" && cmd != "PRIVMSG" \
            && cmd != "PASS" && cmd != "USER" && cmd != "NICK" && cmd != "PING" && cmd != "PONG" ) {
        msg = ":ircserv_KAI.chat 421 " + client.GetNickname() + ' ' + cmd + " :Unknown command\r\n";
        msg_client(client.GetFdClient(), msg);
    } else if (cmd == "PASS" || cmd == "USER") {
        msg = ":ircserv_KAI.chat 462 " + client.GetNickname() + " :You may not reregister\r\n";
        msg_client(client.GetFdClient(), msg);
    } else if (cmd == "NICK") {
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
        if (part_cmd.empty()) {
            msg = ":ircserv_KAI.chat 431 " + client.GetNickname() + " NICK :No nickname given\r\n";
            msg_client(client.GetFdClient(),msg);
            return;
        }
        std::map<int, Clients>::iterator it;
        for ( it = map_of_clients.begin(); it != map_of_clients.end(); it++) {
            if (it->second.GetNickname() == part_cmd) {
                msg = ":ircserv_KAI.chat 433 " + part_cmd + " NICK :Nickname is already in use\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            }
        }
        client.setNickname(part_cmd);
        for ( it = map_of_clients.begin(); it != map_of_clients.end(); it++) {
                msg = ":" + old_nick + "!~" + client.GetUsername() + '@' + client.GetIpClient() + " NICK :" + client.GetNickname() + "\r\n";
                msg_client(it->second.GetFdClient(), msg);
        }
    }
    return;
}