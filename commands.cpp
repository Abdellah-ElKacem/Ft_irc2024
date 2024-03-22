/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aen-naas <aen-naas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 17:20:03 by aen-naas          #+#    #+#             */
/*   Updated: 2024/03/21 20:55:12 by aen-naas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

void ft_handle_topic(channel & test, std::map<int ,Clients>::iterator& , std::vector<std::string> &args)
{
	if (args.size() == 2)
	{
		if (test._is_topiced)
		{
			std::cout << test._topic_name << std::endl;
		}
		else
			std::cout << "no topic for this channel" << std::endl;
	}
	else if (args.size() > 2)
	{
		test._is_topiced = true;
		test._topic_name = args[1];
	}
	else
	{
		std::cout << "not enough arguments" << std::endl;
	}
	std::cout << args.size() << std::endl;
}

void ft_handle_cmd(std::map<int ,Clients>::iterator& it, std::vector<std::string> &args)
{
	channel test;
	if (args[0] == "TOPIC")
		ft_handle_topic(test,it, args);
}