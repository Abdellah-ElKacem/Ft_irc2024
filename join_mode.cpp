#include "channel.hpp"

std::map<std::string, channel> _channel_list;

void    split_ch(std::string& command, std::vector<std::string>& args)
{
	std::string temp;
	size_t i = 0;
    while (i < command.size())
    {
        if (command[i] == 44)
		{
			args.push_back(temp);
			temp = "";
			while (i < command.size() && command[i] == 44)
				i++;
		}
		else
		{
			temp.push_back(command[i]);
			i++;
		}
    }
	if (!temp.empty())
        args.push_back(temp);
}

void pars_join_mode(std::vector<std::string> cmd)
{
    std::vector<std::string> name;
    std::vector<std::string> pass;
    if (cmd[0] == "JOIN")
    {
        if (cmd.size() >= 2)
        {
            split_ch(cmd[1], name);
            if (cmd.size() >= 3)
                split_ch(cmd[2], pass);
            
        }
    }
    // else if (cmd[0] == "JOIN" )
    // {}
}