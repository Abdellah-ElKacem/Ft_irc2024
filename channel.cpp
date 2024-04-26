#include "channel.hpp"
#include "Clients.hpp"

std::map<std::string, channel> _channel_list;
std::string server_name = "ircserv_KAI.chat";


void trim(std::string& str)
{
	size_t bigen = str.find_first_not_of(" \n\t\v");
	size_t last = str.find_last_not_of(" \n\t\v");
	if (last == std::string::npos)
		last = str.length() -1;
	if (bigen == std::string::npos)
		bigen = 0;
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

void Server::bot(Clients &it) {
	int fd_bot = socket(AF_INET, SOCK_STREAM, 0);
	__clients.sin_family = AF_INET;
    __clients.sin_port = htons(_port);
    __clients.sin_addr.s_addr = INADDR_ANY;
	std::cout << fd_bot << std::endl;
	connect(fd_bot, (struct sockaddr*)&__clients, sizeof(__clients));
	it.SetBoolBot(true);
	_bot_fd = fd_bot;
}

void check_cmd(std::map<int ,Clients>::iterator it)
{
    std::vector<std::string> args;
	std::string command = it->second.GetBuffer();
	// std::cout << command << std::endl;
	if (command.empty())
		return;
    ft_split_command(command, args);
	// std::cout << it->second.GetBuffer() << std::endl;
	for (size_t i = 0; i < args[0].length(); i++)
	{
		args[0][i] = std::toupper(args[0][i]);
	}
	// for (size_t i = 0; i < args.size(); i++)
	// {
	// 	std::cout << args[i] << std::endl;
	// }
    if (args[0] == "JOIN" || args[0] == "MODE")
		pars_join_mode(args, it);  
	if (args[0] == "KICK" || args[0] == "INVITE" || args[0] == "TOPIC" || args[0] == "PRIVMSG")
		ft_handle_cmd(it, args);
	// if (it->second.GetBoolBot() == false && args[0] == "/BOT") {
	// 	bot(it);
	// 	return;
	// }
					// if (it->second.GetBoolBot() == true && args[0] == "/JOKE") {
					// 	std::string msg = "galik hada wa7ad hahaha xD \n";
					// 	send_rep(it->first, msg);
					// }
	// std::map<std::string, channel>::iterator pr;
	// for (pr = _channel_list.begin(); pr != _channel_list.end(); pr++)
	// {
	// 	std::cout << "channel name --> " << pr->first << std::endl;
	// 	for (size_t i = 0; i < pr->second._members_list.size(); i++)
	// 	{
	// 		std::cout << "operetors list --> " << pr->second._members_list[i] << std::endl;
	// 		std::cout << "an the nickname list --> " << pr->second._members_list1[i].first << " - "<< pr->second._members_list1[i].second << std::endl;
	// 	}
		

	// }
	// std::cout << "-----------\n";
}
// 05:25 <alae> Trying file transfer to test, Screen Shot 2023-12-26 at 8.55.38 AM.png (6550500 bytes) 127.0.0.1:1098
// 05:25 <test> Received file transfer request from alae, Screen_Shot_2023-12-26_at_8.55.38_AM.png (6550500 bytes) 127.0.0.1:1098

// 05:27 <alae> Trying file transfer to test, Screen Shot 2023-12-26 at 8.55.38 AM.png (6550500 bytes) 127.0.0.1:1099
// 05:27 <test> Received file transfer request from alae, Screen_Shot_2023-12-26_at_8.55.38_AM.png (6550500 bytes) 127.0.0.1:1099

// Trying file transfer to test, Screen Shot 2023-12-26 at 8.55.38 AM.png (6550500 bytes) 127.0.0.1:1098


// Trying file transfer to transfer, image.jpeg (184344 bytes) 197.230.30.146:1097
// 21:50 Trying file transfer to transfer, Screen Shot 2023-12-26 at 8.55.38 AM.png (6550500 bytes) 197.230.30.146:1096
