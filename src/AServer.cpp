/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 18:39:16 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/11 18:40:10 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AServer.hpp"

Server::Server(int server_index, ConfigParser &configParser, char *env[]): _server_index(server_index), _configParser(configParser), _env(env)
{
	_max_backlog_queue = MAX_BACKLOG_QUEUE;

	std::cout << "Server constructor" << std::endl;

	if (VERBOSE == 1)
	{
		std::cout << "Data from Config" << std::endl;
		std::cout << " Nb of servers " << configParser.getNumServers() << std::endl;
		for (size_t i = 0; i < configParser.getNumServers(); i++)
		{
			std::cout << std::endl;
			std::cout << " -- Server " << i << " -- " << std::endl;
			std::cout << "listen port : " << configParser.getParameterValue(i, "listen") << std::endl;
			std::cout << "server name : " << configParser.getParameterValue(i, "server_name") << std::endl;
		}
	}
}

Server::~Server(void)
{
	std::cout << "Server deconstructor" << std::endl;
}
