#include "../Server.hpp"

void Server::register_client(Clients& client ,std::string &str_m ,std::string &str_d ,std::string &str_y ,std::string &str_h ,std::string &str_mi ,std::string &str_s) {

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
            if (part2.c_str()[0] == ':')
                part2.erase(0, 1);
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
            if (part1 == "PASS" && client.GetBoolPassword() == true) {
                msg = ":ircserv_KAI.chat * " + client.GetNickname() + " PASS :You are already put the password\r\n";
                msg_client(client.GetFdClient(),msg);
                return;
            } if ( part1 == "NICK" ) {
                if (client.GetBuffer().find(" ") != client.GetBuffer().npos) {
                    part2 = client.GetBuffer().substr(client.GetBuffer().find(" ") + 1);
                    if (part2.back() == ':')
                        part2.pop_back();
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
                msg = ":ircserv_KAI.chat 001 " + client.GetNickname() + " :Welcome to the KAI_IRC Network, " + client.GetNickname() + " \r\n";                
                msg_client(client.GetFdClient(), msg);

                msg = ":ircserv_KAI.chat 002 " + client.GetNickname() + " :Your host is ircserv_KAI.chat, running version Vol:°01.. ©™ \r\n";
                msg_client(client.GetFdClient(), msg);

                msg = ":ircserv_KAI.chat 003 " + client.GetNickname() + " :This server was created " + str_m + '/' + str_d + "/" + str_y + " at " + str_h + ':' + str_mi + ':' + str_s + " GMT \r\n";
                msg_client(client.GetFdClient(), msg);

                msg = ":ircserv_KAI.chat 004 " + client.GetNickname() + " :host: ircserv_KAI.chat, Version: Vol:°01.. ©™, User mode: none, Channel modes: o, t, k, i !\r\n";
                msg_client(client.GetFdClient(), msg);

                client.SetBoolIdentify(true);
                nick_clients.insert(std::make_pair(client.GetNickname(), client));
                                            // std::cout << "------> " << client.GetNickname() << " | " << client.GetFdClient() << " | "<< client.GetUsername() << std::endl;
            }
        }
    }
}

void Server::time_server(std::string &str_m ,std::string &str_d ,std::string &str_y ,std::string &str_h ,std::string &str_mi ,std::string &str_s) {
    std::ostringstream to_str1, to_str2, to_str3, to_str4, to_str5, to_str6;

    time_t now = time(0);
    tm *local_time = localtime(&now);

    int year = 1900 + local_time->tm_year, month = 1 + local_time->tm_mon , day = local_time->tm_mday;
    int hour = local_time->tm_hour, minute = local_time->tm_min, second = local_time->tm_sec;
    to_str1 << year;
    str_y = to_str1.str();
    to_str2 << month;
    str_m = to_str2.str();
    to_str3 << day;
    str_d = to_str3.str();
    to_str4 << hour;
    str_h = to_str4.str();
    to_str5 << minute;
    str_mi = to_str5.str();
    to_str6 << second;
    str_s = to_str6.str();
}