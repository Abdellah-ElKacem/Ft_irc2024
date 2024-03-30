#include "../Server.hpp"

void Server::register_client(Clients& client) {

    std::string part1, part2, part5 = "", msg;
    std::string after_regis[6] = {"JOIN", "KICK", "INVITE", "TOPIC", "MODE", "PRIVMSG"};

    if (client.GetBoolOk() == false) {
        part1 = client.GetBuffer().substr(0,client.GetBuffer().find(" "));
        for (size_t i = 0; i < part1.size(); i++) {
            part1[i] = std::toupper(part1.c_str()[i]);
        } if (part1 == "JOIN" || part1 == "KICK" || part1 == "INVITE" || part1 == "TOPIC" || part1 == "MODE" || part1 == "PRIVMSG") {
            int index = switch_aft(part1);
            msg = ":ircserv_KAI.chat 451 " + client.GetNickname() + ' ' + after_regis[index] + " :You have not registered\r\n";
            msg_client(client.GetFdClient(), msg);
            return;
        }
        if ( part1 == "PASS" && client.GetBoolPassword() == false ) {
            if (client.GetBuffer().find(" ") != client.GetBuffer().npos)
                part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
            else
                part2 = "";
            if (part2.empty()) {
                msg = ":ircserv_KAI.chat 461 " + client.GetNickname() + " PASS :Not enough parameters\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            } if (part2 != _password) {
                msg = ":ircserv_KAI.chat 464 " + client.GetNickname() + " PASS :Password incorrect\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            }
            client.SetBoolPassword(true);
            return;
        } else if ( client.GetBoolPassword() == true ) {
            // if (part1 == "PASS" && client.GetBoolPassword() == true) {
            //         msg = ":ircserv_KAI.chat -1 " + client.GetNickname() + " PASS :You are already put the password\r\n";
            //     msg_client(client.GetFdClient(),msg);
            //     return;
            // }
            if ( part1 == "NICK" ) {
                if (client.GetBuffer().find(" ") != client.GetBuffer().npos) {
                    part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
                    if(!parce_nick(part2)) {
                        msg = ":ircserv_KAI.chat 432 " + client.GetNickname() + " NICK :Erroneus nickname\r\n";
                        msg_client(client.GetFdClient(),msg);
                        return;
                    }
                }
                else
                    part2 = "";
                if (part2.empty()) {
                    msg = ":ircserv_KAI.chat 431 " + client.GetNickname() + " NICK :No nickname given\r\n";
                    msg_client(client.GetFdClient(),msg);
                    return;
                }
                std::map<int, Clients>::iterator it;
                for ( it = map_of_clients.begin(); it != map_of_clients.end(); it++) {
                    if (it->second.GetNickname() == part2) {
                        msg = ":ircserv_KAI.chat 433 " + part2 + " NICK :Nickname is already in use\r\n";
                        msg_client(client.GetFdClient(),msg);
                        return;
                    }
                }
                client.setNickname(part2);
                client.SetBoolNickname(true);
            } else if ( part1 == "USER" ) {
                if (client.GetBuffer().find(" ") != client.GetBuffer().npos) {
                    part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
                    if (for_iden_user(part2, part5) == 1){
                        msg = ":ircserv_KAI.chat 461 " + client.GetNickname() + " USER :Not enough parameters\r\n";
                        msg_client(client.GetFdClient(),msg);
                        return;
                    }
                } else
                    part2 = "";
                if (part2.empty()) {
                    msg = ":ircserv_KAI.chat 461 " + client.GetNickname() + " USER :Not enough parameters\r\n";
                    msg_client(client.GetFdClient(),msg);
                    return;
                }
                client.setUsername(part2);
                client.setRealname(part5);
                if (client.GetRealname().empty()) {
                    msg = ":ircserv_KAI.chat 461 " + client.GetNickname() + " USER :Not enough parameters\r\n";
                    msg_client(client.GetFdClient(),msg);
                    return;
                }
                client.SetBoolUsername(true);
            }
            if (client.GetBoolPassword() == true && client.GetBoolNickname() == true && client.GetBoolUsername() == true) {
                std::cout << "OK!...\n";
                std::cout << "the client register on the ircserv_KAI.chat server and his NickName is : < " << client.GetNickname() << " > and realname is : < " << client.GetRealname() << " >\n";
                msg_client(client.GetFdClient(),"U are registed, enjoy...\n");
                client.SetBoolIdentify(true);
            }
        }
    }
}
