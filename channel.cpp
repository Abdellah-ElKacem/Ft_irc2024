#include "channel.hpp"
#include "Clients.hpp"

void check_cmd(std::map<int ,Clients>::iterator it)
{
    if ( (it->second.GetBuffer()).substr(0, 4) == "JOIN" )
        std::cout << "JOIN CMD\n";
    else if ( (it->second.GetBuffer()).substr(0, 4) == "MODE" )
        std::cout << "cmd not found\n";
}