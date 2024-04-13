/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux_helper.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 09:05:10 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:08:02 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_OS__linux.hpp"

// Checks if this port was already set as a socket : usage to avoid duplicated 
// initialization in case of more than one servers using the same port 
int	ServerOS::_is_port_already_set(int port, int *array_ports_set, int size_array)
{
	for (int i = 0; i < size_array; i++)
	{
		if (array_ports_set[i] == port)
			return (i);
	}
	return (-1);
}

// Check if key exists in a map
bool	ServerOS::_is_key_in_map(std::map<int, Connection *> *map, int key)
{
	if (map->find(key) == map->end())
		return (false);
	return (true);
}

// Check if the passed file descriptor belongs to a server socket
bool	ServerOS::_is_a_server_socket(int fd) const
{
	if (fd <= 2)
		return (false);

	for (int i = 0; i < _nb_of_servers; i++)
	{
		if (fd == _servers_fd[i])
		{
			return (true);
		}
	}
	return (false);
}


// For debuging
void	ServerOS::_print_epoll_events(uint32_t event, int fd)
{
	if (event & EPOLLERR)
		std::cout << "EPOLLERR: " << fd << std::endl;

	else if (event & EPOLLIN)
		std::cout << "EPOLLIN: " << fd << std::endl;

	else if (event & EPOLLOUT)
		std::cout << "EPOLLOUT: " << fd << std::endl;

	else if (event & EPOLLRDHUP)
		std::cout << "EPOLLRDHUP: " << fd << std::endl;

	else if (event & EPOLLHUP)
		std::cout << "EPOLLHUP: " << fd << std::endl;
}

