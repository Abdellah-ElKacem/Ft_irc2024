#include "channel.hpp"
#include "Clients.hpp"

std::map<std::string, channel> _channel_list;

void trim(std::string& str)
{
	size_t bigen = str.find_first_not_of(" \n\t\v");
	size_t last = str.find_last_not_of(" \n\t\v");
	if (last == std::string::npos)
		last = str.length() -1;
	if (bigen == std::string::npos)
		bigen = 0;
	// exit(0);
	str =  str.substr(bigen , (last - bigen + 1));
}

void    ft_split_command(std::string& command, std::vector<std::string>& args)
{
	trim(command);
	std::string temp;
	size_t i = 0;
    while (i < command.size())
    {
        if ((command[i] >= 9 && command[i] <= 13) || command[i] == 32)
		{
			args.push_back(temp);
			temp = "";
			while (i < command.size() && ((command[i] >= 9 && command[i] <= 13) || command[i] == 32))
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

void check_cmd(std::map<int ,Clients>::iterator it)
{
    std::vector<std::string> args;
	std::string command = it->second.GetBuffer();
    ft_split_command(command, args);

	for (size_t i = 0; i < args[0].length(); i++)
	{
		args[0][i] = std::toupper(args[0][i]);
	}
    if (args[0] == "JOIN" || args[0] == "MODE")
		pars_join_mode(args, it);


	std::map<std::string, channel>::iterator pr;
	for (pr = _channel_list.begin(); pr != _channel_list.end(); pr++)
	{
		std::cout << "channel name --> " << pr->first << std::endl;
		std::cout << "operetors list --> " << pr->second._operetos_list[0] << std::endl;

		std::cout << "-----------\n";
	}
	// else if (args[0] == "KICK" || args[0] == "INVITE" || args[0] == "TOPIC")
	// 	// ft_handle_cmd(it, args);
}