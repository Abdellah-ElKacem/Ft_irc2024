/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-kace <ael-kace@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 17:20:03 by aen-naas          #+#    #+#             */
/*   Updated: 2024/05/08 16:10:42 by ael-kace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

bool ft_check_list(std::vector<std::string>& vec, std::string name)
{
	vector_it it2 = std::find(vec.begin(), vec.end(), name);

	if (it2 != vec.end())
		return true;
	return false;
}
void    split_character(std::string& command, std::vector<std::string>& args)
{
	std::string temp;
	size_t i = 0;
    while (i < command.size())
    {
        if (command[i] == ',')
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

bool ft_check_client(std::vector<std::string>& args)
{
	std::map<std::string, Clients>::iterator it;
	for (size_t i = 1; i < args.size() - 1; i++)
	{
		it = nick_clients.find(args[i]);
		if (nick_clients.end() == it)
			return false;
	}
	return true;
}


bool ft_check_clients(std::string name)
{
	std::map<std::string, Clients>::iterator it;
	it = nick_clients.find(name);
	if (it == nick_clients.end())
		return false;
	return true;
}

void ft_extract_long_line(std::string& line, std::vector<std::string>& args, size_t x)
{
	for (size_t i = x; i < args.size(); i++)
	{
		for (size_t j = 0; j < args[i].length(); j++)
		{
			if (i == x && j == 0 && args[i][j] == ':')
				j++;
			line.push_back(args[i][j]);
		}
		if (i != x && i < args.size() - 1)
			line.push_back(' ');
	}
}


void ft_extract_message(std::string& message, std::vector<std::string>& args)
{
	if (args[2][0] == ':')
		ft_extract_long_line(message, args, 2);
	else
		message = args[2];
}

void ft_remove_fromlist(std::vector<std::string>& list, std::string& name)
{
	vector_it it = std::find(list.begin(), list.end(), name);

    if (it!= list.end())
        list.erase(it);
}

void ft_send_to_all(std::string msg, channels it)
{
	std::map<std::string, Clients>::iterator it_clients;

	for (size_t i = 0; i < it->second._members_list.size(); i++)
	{
		it_clients = nick_clients.find(it->second._members_list[i]);
		send_rep(it_clients->second.GetFdClient(), msg);
	}
	
}



void ft_handle_topic(client& it, std::vector<std::string> &args)
{
	channels	channel_it;
	std::string	long_line;

	if (args.size() == 1 || (args[1].length() == 1 && args[1][0] == ':'))
	{
		send_rep(it->second.GetFdClient(), ERR_NEEDMOREPARAMS(it->second.GetNickname(), server_name, args[0]));
		return ;
	}
	if (args.size() > 1)
	{
		channel_it = _channel_list.find(args[1]);
		if (channel_it == _channel_list.end())
		{
			send_rep(it->second.GetFdClient(), ERR_NOSUCHCHANNEL(it->second.GetIpClient() , it->second.GetNickname(), args[1]));
			return ;
		}
		else if (!ft_check_list(channel_it->second._members_list, it->second.GetNickname()))
		{
			send_rep(it->second.GetFdClient(), ERR_NOTONCHANNEL(it->second.GetIpClient(), args[1]));
			return ;
		}
	}
	if (args.size() == 2)
	{
		if (!channel_it->second._topic_name.empty())
			send_rep(it->second.GetFdClient(), RPL_VIEWTOPIC(server_name, it->second.GetNickname(), channel_it->second._ch_name, channel_it->second._topic_name));
		else
		    send_rep(it->second.GetFdClient(), RPL_NOTOPIC(server_name, it->second.GetNickname(), channel_it->second._ch_name));
		return ;
	}
	if (!channel_it->second._is_topiced || ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
	{
		if (args.size() == 3 && args[2] == ":")
			channel_it->second._topic_name = "";
		else
		{
			if (args[2][0] != ':')
				channel_it->second._topic_name = args[2];
			else
			{
				ft_extract_long_line(long_line, args, 2);
				channel_it->second._topic_name = long_line;
			}
			ft_send_to_all(RPL_SETTOPIC(it->second.GetNickname() ,it->second.GetUsername(), it->second.GetIpClient() ,channel_it->second._ch_name, channel_it->second._topic_name), channel_it);
			channel_it->second._topic_setter = it->second.GetNickname();
		}
	}
	else
        send_rep(it->second.GetFdClient(), ERR_NOTOP(it->second.GetIpClient(), channel_it->second._ch_name));

	
}



void	ft_handle_kick(client& it , std::vector<std::string> &args)
{
	channels channel_it;
	vector_it search_it;
	std::string line;
	if (args.size() <= 2)
	{
		send_rep(it->second.GetFdClient(), ERR_NEEDMOREPARAMS(server_name, it->second.GetNickname(), args[0]));
		return;
	}
	channel_it = _channel_list.find(args[1]);
	if (channel_it == _channel_list.end())
		send_rep(it->second.GetFdClient(),ERR_NOSUCHCHANNEL(server_name, it->second.GetNickname(), args[1]));
	else if (!ft_check_list(channel_it->second._members_list, args[2]) && ft_check_clients(args[2]))
		send_rep(it->second.GetFdClient(), ERR_USERNOTINCHANNEL(server_name, it->second.GetNickname(), args[2], channel_it->second._ch_name));
	else if (!ft_check_list(channel_it->second._members_list, it->second.GetNickname()) && ft_check_clients(it->second.GetNickname()))
		send_rep(it->second.GetFdClient(), ERR_NOTONCHANNEL(server_name, channel_it->second._ch_name));
	else if (!ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
		send_rep(it->second.GetFdClient(), ERR_CHANOPRIVSNEEDED(server_name, it->second.GetNickname(), channel_it->second._ch_name));
	else
	{
		if (!ft_check_clients(args[2]))
		{
			send_rep(it->second.GetFdClient(),ERR_NO_NICK_CHNL(server_name, it->second.GetNickname(), args[2]));
			return ;
		}
		if ((args.size() == 4 && args[3].length() == 1 && args[3][0] == ':') || args.size() == 3)
			ft_send_to_all(RPL_KICK(it->second.GetNickname(), it->second.GetUsername(), it->second.GetIpClient(), channel_it->second._ch_name,  args[2], args[2]), channel_it);
		else if (args.size() >= 4 && args[3][0] != ':')
			ft_send_to_all(RPL_KICK(it->second.GetNickname(), it->second.GetUsername(), it->second.GetIpClient(), channel_it->second._ch_name,  args[2], args[3]), channel_it);
		else
		{
			ft_extract_long_line(line, args, 3);
			ft_send_to_all(RPL_KICK(it->second.GetNickname(), it->second.GetUsername(), it->second.GetIpClient(), channel_it->second._ch_name,  args[2], line), channel_it);
		}
		ft_remove_fromlist(channel_it->second._members_list, args[2]);
		ft_remove_fromlist(channel_it->second._operetos_list, args[2]);
		ft_remove_fromlist(channel_it->second._invited_list, args[2]);
		channel_it->second._members_list1.erase(args[2]);
		if (channel_it->second._members_list.size() == 0)
			_channel_list.erase(channel_it);
	}
}
void	ft_handle_invite(client& it , std::vector<std::string> &args)
{
	channels channel_it;
	std::map<std::string ,Clients>::iterator sender;
	if (args.size() <= 2)
	{
		send_rep(it->second.GetFdClient(), ERR_NEEDMOREPARAMS(it->second.GetIpClient(), it->second.GetNickname(), args[0]));
		return ;
	}
	channel_it = _channel_list.find(args[2]);
	if (channel_it != _channel_list.end() && !ft_check_list(channel_it->second._members_list, it->second.GetNickname()))
	    send_rep(it->second.GetFdClient(), ERR_NOTONCHANNEL(server_name , args[2]));
	else if (!ft_check_client(args))
		send_rep(it->second.GetFdClient(),ERR_NOSUCHNICK(server_name, args[1]));
	else if (channel_it == _channel_list.end())
	    send_rep(it->second.GetFdClient(), ERR_NOSUCHCHANNEL(server_name , it->second.GetNickname(), args[2]));
	else if (ft_check_list(channel_it->second._members_list, args[1]))
	    send_rep(it->second.GetFdClient(), ERR_USERONCHANNEL(server_name , it->second.GetNickname(), args[1], channel_it->second._ch_name));
	else if (!ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
		send_rep(it->second.GetFdClient(), ERR_CHANOPRIVSNEEDED(server_name, it->second.GetNickname(), channel_it->second._ch_name));
	else
	{
		if (!ft_check_list(channel_it->second._invited_list, args[1]))
			channel_it->second._invited_list.push_back(args[1]);
		sender = nick_clients.find(args[1]);
		send_rep(it->second.GetFdClient(), RPL_INVITING(server_name, it->second.GetNickname(), args[1], args[2]));
		send_rep(sender->second.GetFdClient(), RPL_INVITE(it->second.GetNickname(), it->second.GetUsername(), sender->second.GetIpClient(), args[1], args[2]));
	}
}


void	msg_chennel(channels& it_channels, std::string& msg, client&  sender)
{
	vector_it memeber = std::find(it_channels->second._members_list.begin(), it_channels->second._members_list.end(), sender->second.GetNickname());

	std::string recivers_name;
	std::map<std::string, Clients>::iterator recivers_fd;
	if (memeber == it_channels->second._members_list.end())
	{
		send_rep(sender->second.GetFdClient(), ERR_CANNOTSENDTOCHAN(server_name, sender->second.GetNickname(), it_channels->second._ch_name));
		return ;
	}

	for (size_t i = 0; i < it_channels->second._members_list.size(); i++)
	{
		recivers_name = it_channels->second._members_list[i];
		recivers_fd = nick_clients.find(recivers_name);
		if (recivers_name != sender->second.GetNickname())
			send_rep(recivers_fd->second.GetFdClient(), PRIVMSG_FORMAT(sender->second.GetNickname(), sender->second.GetUsername(), sender->second.GetIpClient(), it_channels->second._ch_name, msg));
	}
}

void ft_handle_privmsg(client&  sender, std::vector<std::string> &args)
{
	
	std::map<std::string, Clients>::iterator it_clients;
	channels it_channels;
	std::vector<std::string> senders;
	std::string message;

	if (args.size() == 1)
	{
		send_rep(sender->second.GetFdClient(), ERR_NORECIPIENT(server_name, sender->second.GetNickname(), args[0]) );
        return ;
	}
	else if (args.size() == 2 || (args.size() == 3 && args[2] == ":") )
	{
        send_rep(sender->second.GetFdClient(), ERR_NOTEXTTOSEND(server_name, sender->second.GetNickname()));
        return ;
    }
	split_character(args[1], senders);
	ft_extract_message(message, args);
	for (size_t i = 0; i < senders.size(); i++)
	{
		it_clients = nick_clients.find(senders[i]);
		it_channels = _channel_list.find(senders[i]);
		
		if (it_channels == _channel_list.end() &&  it_clients == nick_clients.end())
			send_rep(sender->second.GetFdClient(), ERR_NO_NICK_CHNL(server_name, senders[i], args[2]));
		else
		{
			if (senders[i][0] == '#')
				msg_chennel(it_channels, message, sender);
			else
				send_rep(it_clients->second.GetFdClient(), PRIVMSG_FORMAT(sender->second.GetNickname(), sender->second.GetUsername(), sender->second.GetIpClient(), it_clients->second.GetNickname(), message));
		}
	}		
}

void ft_handle_cmd(client& it, std::vector<std::string> &args)
{
	if (args[0] == "TOPIC")
		ft_handle_topic(it ,args);
	else if (args[0] == "KICK")
		ft_handle_kick(it , args);
	else if (args[0] == "INVITE")
		ft_handle_invite(it , args);
	else if (args[0] == "PRIVMSG")
		ft_handle_privmsg(it , args);
}