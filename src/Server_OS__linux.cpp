/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 16:31:13 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:24:15 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Server_OS__linux.hpp"

// Parametric Constructor
ServerOS::ServerOS(int server_index, ConfigParser &configParser, char *env[]): Server(server_index, configParser, env), _nb_of_servers(configParser.getNumServers())
{
	std::string		key;
	std::string		value;
	std::string		ident;
	int				*port_array;
	int 			port;
	std::string 	identity;

	_flags_recv = 0;

	// create an epoll instance in a file descriptor
	_epoll_fd = epoll_create(1);	 // epoll_create argument is obsolete and must be >0
	if (_epoll_fd == -1)
	{
		std::cerr << REDB << "Error to epoll_create: " << strerror(errno)<< RESET << std::endl;
		throw ServerCriticalError();
	}

	// clear and initialize struct epoll_event
	clear_memory((void *)&_ev_server, sizeof(_ev_server));
	_ev_server.events =  EPOLLIN | EPOLLOUT;


	// to store the file descriptors of the servers socket per port
	_servers_fd = new int[_nb_of_servers];


	// loop through servers
	// check if a port was already initialized otherwise intialize
	// map port_server_name to a file descriptor

	// intialize port array
	port_array = new int[_nb_of_servers];
	for (int i = 0; i < _nb_of_servers; i++)
	{
		port_array[i] = -1;
	}

	for (int i = 0; i < _nb_of_servers; i++)
	{
		
		port = str2int(configParser.getParameterValue(i, "listen"));
		// we can have doubled ports for different servers, but their socket must be set once per port
		int index = _is_port_already_set(port, port_array, _nb_of_servers);
		if (index == -1) // if new port create a new socket and set _server_fd_port_name
		{
			// setup port
			if (VERBOSE == 1)
			{
				std::cout << std::endl;
				std::cout << "New Port : Setup Socket for port " << port  << std::endl;
			}
			_servers_fd[i]= _setup_socket(port);
			_listen_sockets(_servers_fd[i], port);
			port_array[i] = port;
		}
		else
		{
			if (VERBOSE == 1)
			{
				std::cout << "Already Set : Setup Socket port " << port << std::endl; // remove
			}
			_servers_fd[i]= _servers_fd[index];
		}
	}
	delete [] port_array;
}

ServerOS::~ServerOS(void)
{
	int 		fd;
	Connection *conn;

    // create an map iterator element
	std::map<int, Connection *>::iterator it;

	// clean up any still open file descriptors or Connections
	for (it = _fd2client_map.begin(); it != _fd2client_map.end(); it++)
	{
		fd 	 = it->first;
		conn = it->second;
		// // close file descriptor saved in the key
		close(fd);
		// // free allocated Connection
		delete conn;
	}
	// remove entry from map
	_fd2client_map.clear();

	// close _epoll_fd
	close(_epoll_fd);
	_epoll_fd = -1;

	// close all sockets fds

	for (int i = 0; i < _nb_of_servers; i++)
	{
		_close_server_socket(_servers_fd[i]);
	}

	// free server sockets array
	delete [] _servers_fd;
}

void	ServerOS::_close_connection(int _epoll_fd, int fd_to_remove, struct epoll_event &ev_ref)
{
	if (VERBOSE == 1)
		std::cout << "Remove Connection fd = " << fd_to_remove << std::endl;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd_to_remove, &ev_ref) == -1)
	{
		std::cerr << REDB <<  "Error:\n epoll_ctl connection fd could not be removed from \
			 monitored file descriptor list, fd: " << _epoll_fd << RESET << std::endl;
		std::cerr << "Reason: " << strerror(errno) << std::endl;
		throw ServerCriticalError();
	}
	// delete allocated Connection
	if (VERBOSE == 1)
		std::cout << "file descriptor Connection object free fd: " << fd_to_remove << std::endl;
	delete  _fd2client_map[fd_to_remove];

	// close file descriptor
	if (VERBOSE == 1)
		std::cout << "file descriptor closed  " << fd_to_remove << std::endl;

	close(fd_to_remove);
	// remove from map
	if (VERBOSE == 1)
		std::cout << "file descriptor removed from map " << fd_to_remove << std::endl << std::endl;

	_fd2client_map.erase(fd_to_remove);
}

