/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aen-naas <aen-naas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 17:20:03 by aen-naas          #+#    #+#             */
/*   Updated: 2024/03/22 15:47:46 by aen-naas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

void ft_handle_topic(channel & test, std::map<int ,Clients>::iterator& , std::vector<std::string> &args)
{
	std::cout << args.size() << std::endl;
	if (args.size() == 2)
	{
		if (test._is_topiced)
		{
			std::cout << test._topic_name << std::endl;
		}
		else
			std::cout << "No topic is set" << std::endl;
	}
	else if (args.size() == 3 && args[2] == ":")
	{
		test._is_topiced = false;
		test._topic_name = "";
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
}

void ft_handle_cmd(std::map<int ,Clients>::iterator& it, std::vector<std::string> &args)
{
	channel test;
	test._is_topiced = false;
	if (args[0] == "TOPIC")
		ft_handle_topic(test,it, args);
}