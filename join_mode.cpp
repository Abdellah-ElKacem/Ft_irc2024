#include "channel.hpp"

std::string name_srv = "ircserv_KAI.chat";
std::string used_modes;
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
            // send_rep(it_c->first, RPL_TOPICWHOTIME(name_srv, get_all(it), it->first, it_c->second.GetNickname()));
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

    std::string time = "0";
    

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
            send_rep(it_c->first, RPL_CREATIONTIME(name_srv, it_find->first, it_c->second.GetNickname(), time));
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
void handl_k(std::map<std::string, channel>::iterator it_ch, std::string arg, std::map<int ,Clients>::iterator it_c)
{
    it_ch->second._is_locked = true;
    it_ch->second._pass = arg;

    send_rep(it_c->first, RPL_ADDMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "+k", arg, it_c->second.GetUsername()));
}

void handl_k_m(std::map<std::string, channel>::iterator it_ch, std::map<int ,Clients>::iterator it_c)
{
    it_ch->second._is_locked = false;

    send_rep(it_c->first, RPL_DELMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "-k", it_c->second.GetUsername()));
}

//------- (o) ---------//
void handl_o(std::map<std::string, channel>::iterator it_ch, std::string arg, std::map<int ,Clients>::iterator it_c)
{
    std::vector<std::string>::iterator find_it = std::find(it_ch->second._members_list.begin(), it_ch->second._members_list.end(), arg);
    std::map<std::string, Clients>::iterator it_clients;
    if (find_it != it_ch->second._members_list.end())
        it_ch->second._operetos_list.push_back(*find_it);

    it_clients = nick_clients.find(arg);
    if (it_clients != nick_clients.end())
        send_rep(it_clients->second.GetFdClient(), RPL_ADDMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "+o", arg, it_c->second.GetUsername()));
    send_rep(it_c->first, RPL_ADDMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "+o", arg, it_c->second.GetUsername()));
}

void handl_o_m(std::map<std::string, channel>::iterator it_ch, std::string arg, std::map<int ,Clients>::iterator it_c)
{
    std::vector<std::string>::iterator find_it = std::find(it_ch->second._operetos_list.begin(), it_ch->second._operetos_list.end(), arg);
    if (find_it != it_ch->second._operetos_list.end())
        it_ch->second._operetos_list.erase(find_it);
    send_rep(it_c->first, RPL_DELMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "-o", it_c->second.GetUsername()));
}

//------- (l) ---------//
void handl_l(std::map<std::string, channel>::iterator it_ch, std::string arg, std::map<int ,Clients>::iterator it_c)
{
    if (it_ch->second._limit_members == false)
    {
        it_ch->second._limit_members = true;
        it_ch->second._limit_nb = std::atoi(arg.c_str());
        ft_send_to_all(RPL_ADDMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "+l", arg, it_c->second.GetUsername()), it_ch);
    }
}

void handl_l_m(std::map<std::string, channel>::iterator it_ch, std::map<int ,Clients>::iterator it_c)
{
    if (it_ch->second._limit_members == true)
    {
        it_ch->second._limit_members = false;
        ft_send_to_all(RPL_DELMODE(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "-l", it_c->second.GetUsername()), it_ch);
    }
}

//------- (i) ---------//
void handl_i(std::map<std::string, channel>::iterator it_ch, std::map<int ,Clients>::iterator it_c)
{
    if (it_ch->second._is_invited == false)
    {
        it_ch->second._is_invited = true;
        ft_send_to_all(RPL_ADDMODET(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "+i", it_c->second.GetUsername()), it_ch);
    }
}

void handl_i_m(std::map<std::string, channel>::iterator it_ch, std::map<int ,Clients>::iterator it_c)
{
    if (it_ch->second._is_invited != false)
    {
        it_ch->second._is_invited = false;
        ft_send_to_all(RPL_DELMODET(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "-i", it_c->second.GetUsername()), it_ch);
    }
}

//------- (t) ---------//
void handl_t(std::map<std::string, channel>::iterator it_ch, std::map<int ,Clients>::iterator it_c)
{
    if (it_ch->second._is_topiced == false)
    {
        it_ch->second._is_topiced = true;
        ft_send_to_all(RPL_ADDMODET(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "+t", it_c->second.GetUsername()), it_ch);
    }
}

void handl_t_m(std::map<std::string, channel>::iterator it_ch, std::map<int ,Clients>::iterator it_c)
{
    if (it_ch->second._is_topiced != false)
    {
        it_ch->second._is_topiced = false;
        ft_send_to_all(RPL_DELMODET(it_c->second.GetIpClient(), it_c->second.GetNickname(), it_ch->first, "-t", it_c->second.GetUsername()), it_ch);
    }
}

// ------------------------------------------------------------------------------------// 
// -------------------------------- main modes func -----------------------------------// 
// ------------------------------------------------------------------------------------// 

void change_modes(std::vector<std::string> cmd, std::map<std::string, channel>& _channel_list, std::map<int ,Clients>::iterator it_c)
{
    std::string ch_name = cmd[1];
    std::vector<std::string> modes;
    std::vector<std::string> args;
    std::map<std::string, channel>::iterator it_ch;
    std::vector<std::string>::iterator it_op;
    // int sign = 0;

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
            {
                std::cout << "-- modes -->"<< cmd[i] << std::endl;
                modes.push_back(cmd[i]);
            }
            else
            {
                std::cout << "-- args -->"<< cmd[i] << std::endl;
                args.push_back(cmd[i]);
            }
            z = 1;
        }
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
                        handl_k(it_ch, args[z - 1], it_c);
                    z++;
                }
                else if (modes[0][j] == 'o')
                {
                    
                    if (z <= args.size())
                        handl_o(it_ch, args[z - 1], it_c);
                    z++;
                }
                else if (modes[0][j] == 't')
                {
                    handl_t(it_ch, it_c);
                    z++;
                }
                else if (modes[0][j] == 'l')
                {
                    if (z <= args.size())
                        handl_l(it_ch, args[z - 1], it_c);
                    z++;
                }
                else if (modes[0][j] == 'i')
                {
                    std::cout << "*-*--*-*-\n";
                    handl_i(it_ch, it_c);
                    z++;
                }
                else
                    break;
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
                    std::cout << "-=-=-=-=-=-=-=\n";
                    if (z <= args.size())
                        handl_k_m(it_ch, it_c);
                    z++;
                }
                else if (modes[0][j] == 'o')
                {
                    
                    if (z <= args.size())
                        handl_o_m(it_ch, args[z - 1], it_c);
                    z++;
                }
                else if (modes[0][j] == 't')
                {
                    handl_t_m(it_ch, it_c);
                    z++;
                }
                else if (modes[0][j] == 'l')
                {
                    if (z <= args.size())
                        handl_l_m(it_ch, it_c);
                    z++;
                }
                else if (modes[0][j] == 'i')
                {
                    
                    handl_i_m(it_ch, it_c);
                    z++;
                }
                else
                    break;
            }
        }
    }
    else
        send_rep(it_c->first, ERR_NOSUCHCHANNEL(server_name, it_c->second.GetNickname(), ch_name));
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