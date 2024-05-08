#include "channel.hpp"

std::string name_srv = "ircserv_KAI.chat";
std::string used_modes = "+";
std::string del_modes = "-";
std::string used_args;

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
    {
         std::cerr << "ERROR: send\n";
    }
}

std::string get_all(std::map<std::string, channel>::iterator it)
{
    std::string holder;
    std::vector<std::string>::iterator it_find;

    for (size_t i = 0; i < it->second._members_list.size(); i++)
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
    obj._members_list1[it_c->second.GetNickname()] = it_c->second.GetFdClient();
    obj._limit_nb++;
    buffer.push_back('+');
    buffer.push_back('t');
    _channel_list.insert(std::make_pair(name_ch, obj));

    send_rep(it_c->first, RPL_JOIN(it_c->second.GetNickname(), it_c->second.GetUsername(), name_ch,it_c->second.GetIpClient()));
    send_rep(it_c->first, RPL_MODEIS(name_ch, name_srv, buffer));
    send_rep(it_c->first, RPL_NAMREPLY(name_srv, "@" + it_c->second.GetNickname(), name_ch, it_c->second.GetNickname()));
    send_rep(it_c->first, RPL_ENDOFNAMES(name_srv, it_c->second.GetNickname(), name_ch));
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
                send_rep(it_c->first, ERR_CHANNELISFULL(name_srv, it_c->second.GetNickname(), it->first));
                return ;
            }
        }

        if (it->second._is_invited == true)
        {
            inv_find = std::find(it->second._invited_list.begin(), it->second._invited_list.end(), it_c->second.GetNickname());
            if (inv_find == it->second._invited_list.end())
            {
                send_rep(it_c->first, ERR_INVITEONLY(it_c->second.GetNickname(), name_srv, it->first));
                return ;
            }
        }

        if (it->second._is_locked == true)
        {
            if ((unsigned int)i < pass_wd.size())
            {
                if (it->second._pass != pass_wd[i])
                {
                    send_rep(it_c->first, ERR_BADCHANNELKEY(it_c->second.GetNickname(), name_srv, it->first));
                    return ;
                }
            }
            else
            {
                send_rep(it_c->first, ERR_BADCHANNELKEY(it_c->second.GetNickname(), name_srv, it->first));
                return ;
            }
        }
        it->second._members_list.push_back(it_c->second.GetNickname());
        it->second._members_list1[it_c->second.GetNickname()] = it_c->second.GetFdClient();
        it->second._limit_nb++;
        if (it->second._topic_name.empty())
        {
            ft_send_to_all(RPL_JOIN(it_c->second.GetNickname(), it_c->second.GetUsername(), it->first ,it_c->second.GetIpClient()), it);
            send_rep(it_c->first, RPL_NAMREPLY(name_srv, get_all(it), it->first, it_c->second.GetNickname()));
            send_rep(it_c->first, RPL_ENDOFNAMES(name_srv, it_c->second.GetNickname(), it->first));
        }
        else
        {
            ft_send_to_all(RPL_JOIN(it_c->second.GetNickname(), it_c->second.GetUsername(), it->first ,it_c->second.GetIpClient()), it);
            send_rep(it_c->first, RPL_TOPICDISPLAY(name_srv, it_c->second.GetNickname(), it->first, it->second._topic_name));
            send_rep(it_c->first, RPL_TOPICWHOTIME(it->second._topic_setter, it_c->second.GetNickname(),name_srv, it->first));
            send_rep(it_c->first, RPL_NAMREPLY(name_srv, get_all(it), it->first, it_c->second.GetNickname()));
            send_rep(it_c->first, RPL_ENDOFNAMES(name_srv, it_c->second.GetNickname(), it->first));
        }
    }
}

void show_modes(std::map<int ,Clients>::iterator it_c, std::map<std::string, channel>& _channel_list, std::string channel_mane)
{
    std::map<std::string, channel>::iterator it_find = _channel_list.find(channel_mane);
    std::vector<std::string>::iterator name_find;
    std::string buffer = "";
    
    if (it_find != _channel_list.end())
    {
        name_find = std::find(it_find->second._members_list.begin(), it_find->second._members_list.end(), it_c->second.GetNickname());
        if (name_find != it_find->second._members_list.end())
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
            send_rep(it_c->first, RPL_CHANNELMODEIS(name_srv, it_c->second.GetNickname(), it_find->first, buffer));
            send_rep(it_c->first, RPL_CREATIONTIME(name_srv, it_find->first, it_c->second.GetNickname()));
        }
        else
            send_rep(it_c->first, ERR_NOTONCHANNEL(name_srv, channel_mane));
    }
    else
        send_rep(it_c->first, ERR_NOSUCHCHANNEL(name_srv, it_c->second.GetNickname(), channel_mane));
}

// ------------------------------------------------------------------------------------//
// ------------------------------------ modes -----------------------------------------//
// ------------------------------------------------------------------------------------//

//------- (k) ---------//
void handl_k(std::map<std::string, channel>::iterator it_ch, std::string arg)
{
    it_ch->second._is_locked = true;
    it_ch->second._pass = arg;

    used_modes.push_back('k');
    for(size_t i = 0; i < arg.size(); i++)
        used_args.push_back(arg[i]);
    used_args.push_back(' ');
}

void handl_k_m(std::map<std::string, channel>::iterator it_ch)
{
    it_ch->second._is_locked = false;
    del_modes.push_back('k');
}

//------- (o) ---------//
void handl_o(std::map<std::string, channel>::iterator it_ch, std::string arg, std::map<int ,Clients>::iterator it_c)
{
    std::vector<std::string>::iterator find_it = std::find(it_ch->second._members_list.begin(), it_ch->second._members_list.end(), arg);
    std::map<std::string, Clients>::iterator it_clients;
    if (find_it != it_ch->second._members_list.end())
    {
        it_ch->second._operetos_list.push_back(*find_it);
        it_clients = nick_clients.find(arg);
        if (it_clients != nick_clients.end())
            send_rep(it_clients->second.GetFdClient(), RPL_ADDMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "+o", arg, it_c->second.GetUsername()));
    }
    else
    {
        send_rep(it_c->first, ERR_NO_NICK_CHNL(name_srv, it_c->second.GetNickname(), arg));
        return ;
    }
    used_modes.push_back('o');
    for(size_t i = 0; i < arg.size(); i++) 
        used_args.push_back(arg[i]);
    used_args.push_back(' ');
}

void handl_o_m(std::map<std::string, channel>::iterator it_ch, std::string arg)
{
    std::vector<std::string>::iterator find_it = std::find(it_ch->second._operetos_list.begin(), it_ch->second._operetos_list.end(), arg);
    if (find_it != it_ch->second._operetos_list.end())
        it_ch->second._operetos_list.erase(find_it);
    del_modes.push_back('o');
}

//------- (l) ---------//
void handl_l(std::map<std::string, channel>::iterator it_ch, std::string arg)
{
    if (std::atoi(arg.c_str()) <= 0)
        return ;
    it_ch->second._limit_members = true;
    it_ch->second._limit_nb = std::atoi(arg.c_str());
    used_modes.push_back('l');
    for(size_t i = 0; i < arg.size(); i++)
        used_args.push_back(arg[i]);
    used_args.push_back(' ');
}

void handl_l_m(std::map<std::string, channel>::iterator it_ch)
{
    if (it_ch->second._limit_members == true)
    {
        it_ch->second._limit_members = false;
        del_modes.push_back('l');
    }
}

//------- (i) ---------//
void handl_i(std::map<std::string, channel>::iterator it_ch)
{
    if (it_ch->second._is_invited == false)
    {
        it_ch->second._is_invited = true;
        used_modes.push_back('i');
    }
}

void handl_i_m(std::map<std::string, channel>::iterator it_ch)
{
    if (it_ch->second._is_invited != false)
    {
        it_ch->second._is_invited = false;
        del_modes.push_back('i');
    }
}

//------- (t) ---------//
void handl_t(std::map<std::string, channel>::iterator it_ch)
{
    if (it_ch->second._is_topiced == false)
    {
        it_ch->second._is_topiced = true;
        used_modes.push_back('t');
    }
}

void handl_t_m(std::map<std::string, channel>::iterator it_ch)
{
    if (it_ch->second._is_topiced != false)
    {
        it_ch->second._is_topiced = false;
        del_modes.push_back('t');
    }
}

// ------------------------------------------------------------------------------------// 
// -------------------------------- main modes func -----------------------------------// 
// ------------------------------------------------------------------------------------// 

int check_args_mss(std::vector<std::string> modes, std::vector<std::string> args, std::map<int ,Clients>::iterator it_c)
{
    unsigned int z = 1;
    if (modes[0][0] != '-')
    {
        for(size_t j = 0; j <= modes[0].length() - 1; j++)
        {
            if (modes[0][j] == '+')
            {
                j++;
                if (j >  modes[0].length())
                    break;
            }
            if (modes[0][j] == 'k')
            {
                if (z > args.size())
                {
                    send_rep(it_c->first, ERR_NEEDMOREPARAMS(it_c->second.GetNickname(), name_srv, "MODE"));
                    return 1;
                }
                z++;
            }
            else if (modes[0][j] == 'o')
            {
                if (z > args.size())
                {
                    send_rep(it_c->first, ERR_NEEDMOREPARAMS(it_c->second.GetNickname(), name_srv, "MODE"));
                    return 1;
                }
                z++;
            }
            else if (modes[0][j] == 'l')
            {
                if (z > args.size())
                {
                    send_rep(it_c->first, ERR_NEEDMOREPARAMS(it_c->second.GetNickname(), name_srv, "MODE"));
                    return 1;
                }
                z++;
            }
            else if (modes[0][j] != '-' && modes[0][j] != 'i' && modes[0][j] != 't')
            {
                send_rep(it_c->first, ERR_UNKNOWNMODE(server_name, it_c->second.GetNickname(), modes[0][j]));
                return 1;
            }
        }
    }
    return 0;
}

void change_modes(std::vector<std::string> cmd, std::map<std::string, channel>& _channel_list, std::map<int ,Clients>::iterator it_c)
{
    std::string ch_name = cmd[1];
    std::vector<std::string> modes;
    std::vector<std::string> args;
    std::map<std::string, channel>::iterator it_ch;
    std::vector<std::string>::iterator it_op;
    int st = 0;

    size_t z = 0;
    it_ch = _channel_list.find(ch_name);
    if (it_ch != _channel_list.end())
    {
        it_op = std::find(it_ch->second._operetos_list.begin(), it_ch->second._operetos_list.end(), it_c->second.GetNickname());
        if (it_op == it_ch->second._operetos_list.end())
        {
            send_rep(it_c->first, ERR_NOTOP(name_srv, it_ch->first));
            return ;
        }
        for (size_t i = 2; i <= cmd.size() - 1; i++)
        {
            if (z == 0)
                modes.push_back(cmd[i]);
            else
                args.push_back(cmd[i]);
            z = 1;
        }
        if (check_args_mss(modes, args, it_c) == 1)
            return ;
        z = 1;
        if (modes[0][0] != '-')
        {
            for(size_t j = 0; j <= modes[0].length() - 1; j++)
            {
                if (modes[0][j] == '+')
                {
                    j++;
                    if (j >  modes[0].length())
                        break;
                }
                if (modes[0][j] == 'k')
                {
                    if (z <= args.size())
                        handl_k(it_ch, args[z - 1]);
                    z++;
                }
                else if (modes[0][j] == 'o')
                {
                    if (z <= args.size())
                        handl_o(it_ch, args[z - 1], it_c);
                    z++;
                }
                else if (modes[0][j] == 't')
                    handl_t(it_ch);
                else if (modes[0][j] == 'l')
                {
                    if (z <= args.size())
                        handl_l(it_ch, args[z - 1]);
                    z++;
                }
                else if (modes[0][j] == 'i')
                    handl_i(it_ch);
            }
            if (used_modes.size() > 1)
            {
                ft_send_to_all(RPL_ADDMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, used_modes, used_args, it_c->second.GetUsername()), it_ch);
            }
        }
        else
        {
            for(size_t j = 0; j <= modes[0].length() - 1; j++)
            {
                if (modes[0][j] == '-')
                {
                    j++;
                    if (j >  modes[0].length())
                        break;

                }
                if (modes[0][j] == 'k')
                {
                        handl_k_m(it_ch);
                        if (st == 0)
                            used_args.push_back('*');
                        st++;
                }
                else if (modes[0][j] == 'o')
                {
                    
                    if (z <= args.size())
                        handl_o_m(it_ch, args[z - 1]);
                    z++;
                }
                else if (modes[0][j] == 't')
                    handl_t_m(it_ch);
                else if (modes[0][j] == 'l')
                {
                        handl_l_m(it_ch);
                        if (st == 0)
                            used_args.push_back('*');
                        st++;
                }
                else if (modes[0][j] == 'i')  
                    handl_i_m(it_ch);
                else
                {
                    send_rep(it_c->first, ERR_UNKNOWNMODE(server_name, it_c->second.GetNickname(), modes[0][j]));
                    break;
                }
            }
            std::cout << del_modes.size();
            if (del_modes.size() > 1)
            {
                ft_send_to_all(RPL_ADDMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, del_modes, used_args, it_c->second.GetUsername()), it_ch);
            }
        }
    }
    else
        send_rep(it_c->first, ERR_NOSUCHCHANNEL(server_name, it_c->second.GetNickname(), ch_name));
    used_modes.clear();
    used_modes = "+";
    del_modes.clear();
    del_modes = "-";
    used_args.clear();
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
            send_rep(it_c->first, ERR_NEEDMOREPARAMS(it_c->second.GetNickname(), name_srv, cmd[0]));
    }
    else
    {
        if (cmd.size() == 2)
            show_modes(it_c, _channel_list, cmd[1]);
        else if (cmd.size() > 2)
            change_modes(cmd, _channel_list, it_c);
        else 
            send_rep(it_c->first, ERR_NEEDMOREPARAMS(it_c->second.GetNickname(), name_srv, cmd[0]));
    }
}