#include "Server.hpp"
#include "channel.hpp"

// void Server::bot(Clients& client) {
//     std::string the_cmd, msg;
    
//     msg = ":ircserv_KAI.chat 704 " + client.GetNickname() + " : /|:: **--> ! Help Menu ! <--** ::|\\\r\n";
//     msg_client(client.GetFdClient(), msg);
//     msg = ":ircserv_KAI.chat 705 " + client.GetNickname() + " : For general write -> 1 <- | For Commands write -> 2 <- | For Modes write -> 3 <- | For Exit write -> 4 <-\r\n";
//     msg_client(client.GetFdClient(), msg);
//     char buffer[512];
//     while (1) {
//         std::memset(buffer, 0, sizeof(buffer));
//         int rec = recv(client.GetFdClient(), buffer, sizeof(buffer), 0);
//         the_cmd.clear();
//         the_cmd = buffer;
//         if (rec) {
//             if (the_cmd == "0") {
//                 msg = ":menu\r\n";
//                 msg_client(client.GetFdClient(), msg);
//                 continue;
//             }
//             else if (the_cmd == "1") {
//                 msg = ":general\r\n";
//                 msg_client(client.GetFdClient(), msg);
//                 continue;
//             }
//             else if (the_cmd == "2") {
//                 msg = ":commands\r\n";
//                 msg_client(client.GetFdClient(), msg);
//                 continue;
//             }
//             else if (the_cmd == "3") {
//                 msg = ":modes\r\n";
//                 msg_client(client.GetFdClient(), msg);
//                 continue;
//             }
//             else if (the_cmd == "4") {
//                 msg = ":exit\r\n";
//                 msg_client(client.GetFdClient(), msg);
//                 return;
//             }
//         }
//     }
// }

void Server::bot(Clients& client, channel& chan) {
    std::vector<std::string>::iterator it_nick;
    if (client.GetBuffer() == "/help") {
        for (it_nick = chan._operetos_list.begin(); it_nick != chan._operetos_list.end(); it_nick++)
        {
        }
        
    }
}