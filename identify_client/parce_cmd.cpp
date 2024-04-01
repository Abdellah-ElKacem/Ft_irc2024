#include "../Server.hpp"

bool Server::parce_nick(std::string &part2) {
    if (std::isdigit(part2.c_str()[0]))
        return false;
    for (size_t i = 0; i < part2.length(); i++) {
        if ( !std::isalnum(part2[i]) && part2[i] != '[' && part2[i] != ']' && part2[i] != '{' \
            && part2[i] != '}' && part2[i] != '|' && part2[i] != '\\' && part2[i] )
                return false;
    }
    return true;    
}

int Server::switch_aft(std::string cmd) {
    if(!std::strcmp(cmd.c_str(), "JOIN"))
        return 0;
    if(!std::strcmp(cmd.c_str(), "KICK"))
        return 1;
    if(!std::strcmp(cmd.c_str(), "INVITE"))
        return 2;
    if(!std::strcmp(cmd.c_str(), "TOPIC"))
        return 3;
    if(!std::strcmp(cmd.c_str(), "MODE"))
        return 4;
    return 5;
}

//inet_ntoa

int Server::for_iden_user(std::string &part2, std::string &part5) {
    std::string tmp;
    std::vector <std::string> identi_user;
    size_t idx = part2.find(' ');
    int idx_prv = 0;
    while(idx != part2.npos) {
        tmp = part2.substr(idx_prv, idx - idx_prv);
        identi_user.push_back(tmp);
        idx_prv = idx + 1;
        if (part2[idx_prv] == ':') {
            idx_prv++;
            break;
        }
        idx = part2.find(' ',idx_prv + 1);
        tmp.clear();
    }
    identi_user.push_back(part2.substr(idx_prv));
    if (identi_user.size() < 4)
        return 1;
    part2 = identi_user[0];
    part5 = identi_user[3];
    return 0;
}
