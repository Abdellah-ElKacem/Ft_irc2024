/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aen-naas <aen-naas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 17:20:03 by aen-naas          #+#    #+#             */
/*   Updated: 2024/03/23 16:04:15 by aen-naas         ###   ########.fr       */
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


void ft_handle_topic(client& it, std::vector<std::string> &args)
{
	channels channel_it;

	if (args.size() > 1)
	{
		channel_it = _channel_list.find(args[1]);
		if (channel_it == _channel_list.end())
		{
			std::cout << "No such channel" << std::endl;
			return ;
		}
	}
	if (args.size() == 2)
	{
		if (channel_it->second._is_topiced)
		{
			std::cout << channel_it->second._topic_name << std::endl;
		}
		else
			std::cout << "No topic is set" << std::endl;
	}
	if (ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
	{
		if (args.size() == 3 && args[2] == ":")
		{
			channel_it->second._is_topiced = false;
			channel_it->second._topic_name = "";
		}
		else if (args.size() > 2)
		{
			channel_it->second._is_topiced = true;
			channel_it->second._topic_name = args[1];
		}
		else
		{
			std::cout << "not enough arguments" << std::endl;
		}
	}
	
}

void	ft_handle_kick(client& it , std::vector<std::string> &args)
{
	channels channel_it;
	std::vector<std::string>::iterator search_it;
	if (args.size() <= 2)
	{
		std::cerr << "Not enough parameters" << std::endl;
		return;
	}
	channel_it = _channel_list.find(args[1]);
	if (channel_it == _channel_list.end())
		std::cerr << "No such channel" << std::endl;
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
				std::cout << "KICK " << channel_it->second._ch_name << " " << args[i] << std::endl;
			}
		}	
	}
}
void	ft_handle_invite(client& it , std::vector<std::string> &args)
{
	channels channel_it;
	if (args.size() <= 2)
	{
		std::cout << "Not enough parameters" << std::endl;
		return ;
	}
	channel_it = _channel_list.find(args[2]);
	// if (channel_it == _channel_list.end())
	// 	return ;
	if (channel_it == _channel_list.end())
	    std::cerr << "No such channel" << std::endl;
    else if (!ft_check_list(channel_it->second._operetos_list, it->second.GetNickname()))
        std::cerr << "You don't have enough channel privileges" << std::endl;
	else
		channel_it->second._invited_list.push_back(args[1]);	
}

void ft_handle_cmd(client& it, std::vector<std::string> &args)
{
	// channels channel_it = _channel_list.begin();
	if (args[0] == "TOPIC")
		ft_handle_topic(it ,args);
	else if (args[0] == "KICK")
		ft_handle_kick(it , args);
	else if (args[0] == "INVITE")
		ft_handle_invite(it , args);
}