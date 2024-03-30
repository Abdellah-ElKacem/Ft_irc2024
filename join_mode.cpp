#include "channel.hpp"

std::string name_srv = "ircserv_KAI.chat";
channel::channel()
{}

channel::channel(std::string ch_name)
{
    this->_ch_name = ch_name;
    this->_is_locked = false;
    this->_is_invited = false;
    this->_is_topiced = true;
    this->_limit_members = false;
}

void send_rep(int fd, std::string msg)
{
    if (send(fd, msg.c_str() , msg.size(), 0) == -1)
        std::cerr << "ERROR: send\n";
}

std::string get_all(std::map<std::string, channel>::iterator it)
{
    std::string holder;
    std::vector<std::string>::iterator it_find;

    for (size_t i = 0; i <= it->second._members_list.size(); i++)
    {
        it_find = std::find(it->second._operetos_list.begin(), it->second._operetos_list.end(), it->second._members_list[i]);
        if (it_find != it->second._operetos_list.end())
        {
            holder.append(" @");
            holder.append(it->second._members_list[i]);
        }
        else    
        {
            holder.append(" ");
            holder.append(it->second._members_list[i]);
        }
    }
    return holder;
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
    if (name_ch[0] != '#')
    {
        send_rep(it_c->second.GetFdClient(), ERR_NOSUCHCHANNEL(name_srv, it_c->second.GetNickname(), name_ch));
        return ;
    }
    std::string buffer;
    channel obj(name_ch);
    obj._operetos_list.push_back(it_c->second.GetNickname());
    obj._members_list.push_back(it_c->second.GetNickname());
    obj._limit_nb++;
    buffer.push_back('+');
    buffer.push_back('t');
    _channel_list.insert(std::make_pair(name_ch, obj));

    send_rep(it_c->second.GetFdClient(), RPL_JOIN(it_c->second.GetNickname(), it_c->second.GetUsername(), name_ch,it_c->second.GetIpClient()));
    send_rep(it_c->second.GetFdClient(), RPL_MODEIS(name_ch, name_srv, buffer));
    send_rep(it_c->second.GetFdClient(), RPL_NAMREPLY(name_srv, "@" + it_c->second.GetNickname(), name_ch, it_c->second.GetNickname()));
    send_rep(it_c->second.GetFdClient(), RPL_ENDOFNAMES(name_srv, it_c->second.GetNickname(), name_ch));
}

void join_user_to_channel(std::map<int ,Clients>::iterator it_c, std::map<std::string, channel>::iterator it, std::vector<std::string> pass_wd, int i)
{
    std::vector<std::string>::iterator inv_find;
    std::vector<std::string>::iterator mem_find;

    mem_find = std::find(it->second._members_list.begin(), it->second._members_list.end(), it_c->second.GetNickname());

    if (mem_find == it->second._members_list.end())
    {
        if (it->second._limit_members == true)
        {
            if (it->second._members_list.size() >= it->second._limit_nb)
            {
                send_rep(it_c->second.GetFdClient(), ERR_CHANNELISFULL(it_c->second.GetNickname(), it->first));
                return ;
            }
        }

        if (it->second._is_invited == true)
        {
            inv_find = std::find(it->second._invited_list.begin(), it->second._invited_list.end(), it_c->second.GetNickname());
            if (inv_find == it->second._invited_list.end())
            {
                send_rep(it_c->second.GetFdClient(), ERR_INVITEONLY(it_c->second.GetNickname(), it->first));
                return ;
            }
        }

        if (it->second._is_locked == true)
        {
            if ((unsigned int)i < pass_wd.size())
            {
                if (it->second._pass != pass_wd[i])
                {
                    send_rep(it_c->second.GetFdClient(), ERR_BADCHANNELKEY(it_c->second.GetNickname(), name_srv, it->first));
                    return ;
                }
            }
            else
            {
                send_rep(it_c->second.GetFdClient(), ERR_BADCHANNELKEY(it_c->second.GetNickname(), name_srv, it->first));
                return ;
            }
        }
        it->second._members_list.push_back(it_c->second.GetNickname());
        it->second._limit_nb++;
        ft_send_to_all(RPL_JOIN(it_c->second.GetNickname(), it_c->second.GetUsername(), it->first,it_c->second.GetIpClient()), it);
        send_rep(it_c->second.GetFdClient(), RPL_NAMREPLY(name_srv, get_all(it), it->first, it_c->second.GetNickname()));
        send_rep(it_c->second.GetFdClient(), RPL_ENDOFNAMES(name_srv, it_c->second.GetNickname(), it->first));
    }
}

void show_modes(std::map<int ,Clients>::iterator it_c, std::map<std::string, channel>& _channel_list, std::string channel_mane)
{
    std::map<std::string, channel>::iterator it_find = _channel_list.find(channel_mane);
    std::string buffer;

    // std::time_t time = std::time(NULL);
    std::string time = "0";
    

    if (it_find != _channel_list.end())
    {
        buffer.push_back('+');
        if (it_find->second._is_invited)
            buffer.push_back('i');
        if (it_find->second._is_locked)
            buffer.push_back('k');
        if (it_find->second._is_topiced)
            buffer.push_back('t');
        if (it_find->second._limit_members)
            buffer.push_back('l');
        send_rep(it_c->second.GetFdClient(), RPL_CHANNELMODEIS(name_srv, it_c->second.GetNickname(), it_find->first, buffer));
        send_rep(it_c->second.GetFdClient(), RPL_CREATIONTIME(name_srv, it_find->first, it_c->second.GetNickname(), time));
    }
    else
        send_rep(it_c->second.GetFdClient(), ERR_NOSUCHCHANNEL(name_srv, it_c->second.GetNickname(), it_find->first));
}


void change_modes(std::vector<std::string> cmd, std::map<std::string, channel>& _channel_list)
{
    std::string ch_name = cmd[1];
    std::vector<std::string> modes;
    std::vector<std::string> args;
    std::map<std::string, channel>::iterator it_ch;

    size_t z = 0;
    it_ch = _channel_list.find(ch_name);
    if (it_ch != _channel_list.end())
    {
        for (size_t i = 2; i <= cmd.size() - 1; i++)
        {
            if ((cmd[i][0] == '+' || cmd[i][0] == '-') && z == 0)
                modes.push_back(cmd[i]);
            else
            {
                if ((cmd[i][0] == '+' || cmd[i][0] == '-'))
                    break;
                args.push_back(cmd[i]);
                z = 1;
            }
        }
        // z = 1;
        // for(size_t i = 0; i <= modes.size() - 1; i++)
        // {
        //     for(size_t j = 1; j <= modes[i].length() - 1; j++)
        //     {
        //         if (z <= args.size())
        //         {
        //             if ()
        //             {

        //                 z++;
        //             }
        //         }
        //     }
        // }
    }
    // for (size_t i = 0; i < modes.size(); i++)
    // {
    //     std::cout << "--> mode :" << modes[i] << std::endl;
    // }
    // for (size_t i = 0; i < args.size(); i++)
    // {
    //     std::cout << "--+> args :"<< args[i] << std::endl;
    // }
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
            send_rep(it_c->second.GetFdClient(), ERR_NEEDMOREPARAMS(it_c->second.GetNickname(), name_srv, cmd[0]));
    }
    else
    {
        if (cmd.size() == 2)
            show_modes(it_c, _channel_list, cmd[1]);
        else if (cmd.size() > 2)
            change_modes(cmd, _channel_list);
        else 
            send_rep(it_c->second.GetFdClient(), ERR_NEEDMOREPARAMS(it_c->second.GetNickname(), name_srv, cmd[0]));
    }
}