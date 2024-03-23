#include "channel.hpp"

channel::channel()
{}

channel::channel(std::string ch_name)
{
    this->_ch_name = ch_name;
    this->_is_locked = false;
    this->_is_invited = false;
    this->_is_topiced = false;
    this->_limit_members = false;
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

void creat_channel(std::map<std::string, channel>& _channel_list, std::map<int ,Clients>::iterator it_c, std::string name_ch)
{
    channel obj(it_c->second.GetUsername());
    obj._operetos_list.push_back(it_c->second.GetUsername());
    obj._limit_nb++;

    _channel_list.insert(std::make_pair(name_ch, obj));
}

void join_user_to_channel(std::map<int ,Clients>::iterator it_c, std::map<std::string, channel>::iterator it, std::string pass_wd)
{
    std::vector<std::string>::iterator inv_find;
    (void) pass_wd;
    if (it->second._limit_members == true)
    {
        if (it->second._members_list.size() + it->second._operetos_list.size() == it->second._limit_nb)
        {
            if (it->second._is_invited == true)
            {
                inv_find = std::find(it->second._invited_list.begin(), it->second._invited_list.end(), it_c->second.GetUsername());
                if (inv_find != it->second._invited_list.end())
                {
                    if (it->second._is_locked == true)
                    {
                        if (it->second._pass == pass_wd)
                        {
                            it->second._members_list.push_back(it_c->second.GetUsername());
                            it->second._limit_nb++;
                        }
                    }
                }
            }
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
                split_ch(cmd[2], pass);
            for (loop = name.begin(); loop != name.end(); loop++)
            {
                if (_channel_list.size() == 0)
                    creat_channel(_channel_list, it_c, *loop);
                else
                {
                    it = _channel_list.find(*loop);
                    if (it != _channel_list.end())
                    {
                        std::cout << pass.size() << std::endl;
                        // join_user_to_channel(it_c, it, pass[i]);
                    }
                    else
                        creat_channel(_channel_list, it_c, *loop);
                }
                i++;
            }
        }
    }
}