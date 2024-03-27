#include "channel.hpp"

channel::channel()
{}

channel::channel(std::string ch_name)
{
    this->_ch_name = ch_name;
    this->_is_locked = true;
    this->_is_invited = false;
    this->_is_topiced = false;
    this->_limit_members = false;

    this->_pass = "123";
}

void send_rep(int fd, std::string msg)
{
    if (send(fd, msg.c_str() , msg.size(), 0) == -1)
        std::cerr << "ERROR: send\n";
}

channel::~channel()
{}

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

void    split_ch_pass(std::string& command, std::vector<std::string>& args)
{
	std::string temp;
	size_t i = 0;
    while (i < command.size())
    {
        if(command[i] != 44)
            temp.push_back(command[i]);
        else
        {
            args.push_back(temp);
            temp = "";
        }
        i++;
    }
	if (!temp.empty())
        args.push_back(temp);
}

void creat_channel(std::map<std::string, channel>& _channel_list, std::map<int ,Clients>::iterator it_c, std::string name_ch)
{
    channel obj(name_ch);
    obj._operetos_list.push_back(it_c->second.GetNickname());
    obj._members_list.push_back(it_c->second.GetNickname());
    obj._limit_nb++;
    _channel_list.insert(std::make_pair(name_ch, obj));
}

void join_user_to_channel(std::map<int ,Clients>::iterator it_c, std::map<std::string, channel>::iterator it, std::vector<std::string> pass_wd, int i)
{
    std::vector<std::string>::iterator inv_find;
    std::vector<std::string>::iterator mem_find;
    int is_in_inv = 0;

    mem_find = std::find(it->second._members_list.begin(), it->second._members_list.end(), it_c->second.GetNickname());

    if (mem_find == it->second._members_list.end())
    {
        if (it->second._limit_members == true)
        {
            if (it->second._members_list.size() >= it->second._limit_nb)
                std::cout << "ERROR !!\n";
        }

        if (it->second._is_invited == true)
        {
            inv_find = std::find(it->second._invited_list.begin(), it->second._invited_list.end(), it_c->second.GetNickname());
            if (inv_find != it->second._invited_list.end())
                is_in_inv = 1;
            else
                std::cout << "ERROR !!\n"; 
        }

        if (it->second._is_locked == true)
        {
            if ((unsigned int)i < pass_wd.size())
            {
                if (it->second._pass == pass_wd[i])
                {
                    it->second._members_list.push_back(it_c->second.GetNickname());
                    it->second._limit_nb++;
                }
                else
                    std::cout << "wrong password !!\n";
            }
            else
                std::cout << "missing password !!\n";
        }
    }

}

void pars_join_mode(std::vector<std::string> cmd, std::map<int ,Clients>::iterator it_c)
{
    std::vector<std::string> name;
    std::vector<std::string> pass;
    std::map<std::string, channel>::iterator it;
    std::vector<std::string>::iterator loop;
    int i = 0;

    if (cmd[0] == "JOIN")
    {
        if (cmd.size() >= 2)
        {
            split_ch(cmd[1], name);
            if (cmd.size() >= 3)
                split_ch_pass(cmd[2], pass);
            for (loop = name.begin(); loop != name.end(); loop++)
            {
                if (_channel_list.size() == 0)
                    creat_channel(_channel_list, it_c, *loop);
                else
                {
                    it = _channel_list.find(*loop);
                    if (it != _channel_list.end())
                        join_user_to_channel(it_c, it, pass, i);
                    else
                        creat_channel(_channel_list, it_c, *loop);
                }
                i++;
            }
        }
        else
            std::cout << "ds" << std::endl;
            // send_rep(it_c->second.GetFdClient(), ERR_NEEDMOREPARAMS(it_c->second.GetNickname(), cmd[0]));
    }
}