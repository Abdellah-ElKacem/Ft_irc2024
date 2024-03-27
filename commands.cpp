/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aen-naas <aen-naas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 17:20:03 by aen-naas          #+#    #+#             */
/*   Updated: 2024/03/27 02:20:52 by aen-naas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

bool ft_check_list(std::vector<std::string>& vec, std::string name)
{
	std::vector<std::string>::iterator it2 = std::find(vec.begin(), vec.end(), name);

	if (it2 != vec.end())
		return true;
	return false;
}

// bool ft_check_channel(channels& channel_it, std::string &channel_name)
// {
// 	if (channel_it->second._ch_name == channel_name)
// 		return false;
// 	return true;
// }

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
	channels channel_it;

	if (args.size() == 1)
	{
		send_rep(it->second.GetFdClient(), ERR_NEEDMOREPARAMS(it->second.GetIpClient(), it->second.GetNickname(), args[0]));
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
		else if (!ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
		{
			send_rep(it->second.GetFdClient(), ERR_NOTONCHANNEL(it->second.GetIpClient(), args[1]));
			return ;
		}
	}
	if (args.size() == 2)
	{
		if (!channel_it->second._topic_name.empty())
			send_rep(it->second.GetFdClient(), RPL_VIEWTOPIC(it->second.GetIpClient(), it->second.GetNickname(), channel_it->second._ch_name, channel_it->second._topic_name));
		else
		    send_rep(it->second.GetFdClient(), RPL_NOTOPIC(it->second.GetIpClient(), it->second.GetNickname(), channel_it->second._ch_name));
		return ;
	}
	if (ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
	{
		if (args.size() == 3 && args[2] == ":")
		{
			channel_it->second._is_topiced = false;
			channel_it->second._topic_name = "";
		}
		else if (args.size() == 3)
		{
			channel_it->second._is_topiced = true;
			channel_it->second._topic_name = args[2];
			ft_send_to_all(RPL_TOPIC(it->second.GetIpClient(), it->second.GetNickname(), channel_it->second._ch_name, args[2]), channel_it);
		}
	}
	
}

void	ft_handle_kick(client& it , std::vector<std::string> &args)
{
	channels channel_it;
	std::vector<std::string>::iterator search_it;
	if (args.size() <= 2)
	{
		send_rep(it->second.GetFdClient(), ERR_NEEDMOREPARAMS(it->second.GetIpClient(), it->second.GetNickname(), args[0]));
		return;
	}
	channel_it = _channel_list.find(args[1]);
	if (channel_it == _channel_list.end())
		send_rep(it->second.GetFdClient(),ERR_NOSUCHCHANNEL(it->second.GetIpClient(), it->second.GetNickname(), args[1]));
	else if (!ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
	else if (!ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
		std::cerr << "You don't have enough channel privileges" << std::endl;
	else
	{
		for (size_t i = 2; i < args.size(); i++)
		{
			if (!ft_check_list(channel_it->second._members_list, args[2]))
				std::cerr << "No such nick" << std::endl;
			else
			{
				search_it = std::find(channel_it->second._members_list.begin(), channel_it->second._members_list.end(), args[i]); 
				channel_it->second._members_list.erase(search_it);
				// send_rep(it->second.GetFdClient(), RPL_KICK(it->second.GetNickname(), it->second.GetIpClient(), channel_it->second._ch_name, ))
				std::cout << "KICK " << channel_it->second._ch_name << " " << args[i] << std::endl;
			}
		}	
	}
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
void	ft_handle_invite(client& it , std::vector<std::string> &args)
{
	channels channel_it;
	if (args.size() <= 2)
	{
		send_rep(it->second.GetFdClient(), ERR_NEEDMOREPARAMS(it->second.GetIpClient(), it->second.GetNickname(), args[0]));
		return ;
	}
	channel_it = _channel_list.find(args[2]);
	if (!ft_check_client(args))
		std::cerr << "No such nick" << std::endl;
	else if (channel_it == _channel_list.end())
	    std::cerr << "No such channel" << std::endl;
    else if (!ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
        std::cerr << "You don't have enough channel privileges" << std::endl;
	else
		channel_it->second._invited_list.push_back(args[1]);	
}


void	msg_chennel(channels& it_channels, std::string& msg, client&  sender)
{
	vector_it memeber = std::find(it_channels->second._members_list.begin(), it_channels->second._members_list.end(), sender->second.GetNickname());

	std::string recivers_name;
	std::map<std::string, Clients>::iterator recivers_fd;
	if (memeber == it_channels->second._members_list.end())
	{
		std::cerr << "Cannot send to channel (+n)" << std::endl;
		return ;
	}
	for (size_t i = 0; i < it_channels->second._members_list.size(); i++)
	{
		recivers_name = it_channels->second._members_list[i];
		recivers_fd = nick_clients.find(recivers_name);
		if (recivers_name != sender->second.GetNickname())
		{
			std::cout << msg << std::endl;
			write(recivers_fd->second.GetFdClient(), &msg, msg.length() + 1);
			write(recivers_fd->second.GetFdClient(), "\n\r", 2);
		}
	}
}

void ft_handle_privmsg(client&  sender, std::vector<std::string> &args)
{
	
	std::map<std::string, Clients>::iterator it_clients;
	channels it_channels;

	for (size_t i = 1; i < args.size() - 1; i++)
	{
		it_clients = nick_clients.find(args[i]);
		it_channels = _channel_list.find(args[i]);
		
		if (it_channels == _channel_list.end() && args[i][0] == '#')
		{
			std::cerr << "no such channel" << std::endl;
			return ;
		}
		else if (it_clients == nick_clients.end() && args[i][0] != '#')
		{
			std::cerr << "no such nick" << std::endl;
			return ;
		}
		else
		{
			if (args[i][0] == '#')
				msg_chennel(it_channels, args[args.size() - 1], sender);
			else
			{
				std::cout << args[i] << " : ";
				write(it_clients->second.GetFdClient(), &args[args.size() - 1], args[args.size() - 1].length() + 1);
				write(it_clients->second.GetFdClient(), "\n\r", 2);
			}
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