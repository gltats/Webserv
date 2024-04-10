/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 16:31:13 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/09 21:24:57 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Server_OS__linux.hpp"

int	_is_port_already_set(int port, int *array_ports_set, int size_array)
{
	for (int i = 0; i < size_array; i++)
	{
		if (array_ports_set[i] == port)
			return (i);
	}
	return (-1);
}

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
	_epoll_fd = epoll_create(1);	 // argument is obsolete and must be >0
	if (_epoll_fd == -1)
	{
		std::cerr << REDB << "Error to epoll_create: " << strerror(errno)<< RESET << std::endl;
		throw ServerCriticalError();
	}

	clear_memory((void *)&_ev_server, sizeof(_ev_server));
	_ev_server.events =  EPOLLIN | EPOLLOUT;


	// to store the file descriptors of the servers socket per port
	_servers_fd = new int[_nb_of_servers];


	// loop through servers
	// check if a port was already initialized otherwise intialize it
	// map port_server_name to a file descriptor

	// intialize port array
	port_array = new int[_nb_of_servers];
	for (int i = 0; i < _nb_of_servers; i++)
	{
		port_array[i] = -1;
	}

	for (int i = 0; i < _nb_of_servers; i++)
	{
		std::cout << std::endl;
		port = str2int(configParser.getParameterValue(i, "listen"));
		// we can have doubled ports for different servers, but their socket must be set once per port
		int index = _is_port_already_set(port, port_array, _nb_of_servers);
		if (index == -1) // if new port create a new socket and set _server_fd_port_name
		{
			// setup port
			std::cout << "New Port : Setup Socket for port " << port  << std::endl; // remove
			_servers_fd[i]= _setup_socket(port);
			_listen_sockets(_servers_fd[i], port);
			port_array[i] = port;
		}
		else
		{
			std::cout << "Already Set : Setup Socket port " << port << std::endl; // remove
			_servers_fd[i]= _servers_fd[index];
		}
	}
	delete [] port_array;

}

void	ServerOS::_listen_sockets(int fd_server, int port)
{
	if (listen(fd_server, _max_backlog_queue) == -1)
	{
		//ideally print the server socket and port that could not be listen
		print_error_fd("to listen socket Port ", port);
		close(fd_server);
		//throw exception
		exit (1); // at the moment just exit
	}
	std::cout << "Socket port " << port << " set sucessfully to listen" << std::endl; // remove

}


void	ServerOS::close_server_socket(int fd)
{
	// can we just looop all of them or do i have the need to close only one?
	for (int i = 0; i < _nb_of_servers; i++)
	{
		if (_servers_fd[i] != -1 && _servers_fd[i] == fd)
		{
			close(_servers_fd[i]);
			_servers_fd[i] = -1;
		}
	}
}


void	ServerOS::launch_webserver(void)
{
	// _setup_socket();
	// std::cout << "Socket Setup done" << std::endl;
	// _listen_socket();
	// std::cout << "Socket in listening mode port " << _server_port << std::endl;

	// if (_epoll_fd == 0) // remove
	// 	std::cout << "" << std::endl; // remove
	_loop(); // to be uncommented
}

int		ServerOS::_setup_socket(int port)
{
	int					server_socket;
	struct sockaddr_in	serverAddr;


    // Socket will be created for AF_INET(IPv4), for SOCK_STREAM(TCP) and specific for the TCP protocol
	server_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (server_socket == -1)
	{
        print_error("to create Server Socket");
		throw ServerCriticalError();
	}
	std::cout << "Socket created sucessfully. Socket fd = " << server_socket << std::endl;

    // set socket option to reuse addr otherwise a time must be waited after closing a file descriptor until it can be reused
	int yes = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
        print_error("setsockopt to SO_REUSEADDR");
		throw ServerCriticalError();
	}
	std::cout << "Socket option set to SO_REUSEADDR sucessfully" << std::endl;

    // clear struct memory
	clear_memory(&serverAddr, sizeof(serverAddr));

    // Socket will be binded for IPv4
    serverAddr.sin_family = AF_INET;

    // Socket will not be binded to a specific address (IP). Set as any
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //set the server port as defined in the configuration file
	serverAddr.sin_port = htons(port);

    // Bind socket to configuration defined in struct serverAddr
	if (bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
        print_error("to Bind socket and server_sockaddr");
		print_error(strerror(errno));
		throw ServerCriticalError();
	}
	std::cout << "Binding sucessful socket fd " << server_socket << std::endl;

	// add file descriptor to be monitored
	_ev_server.data.fd = server_socket;

	std::cout << "before epoll_ctl " << _epoll_fd << std::endl; // remove
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, server_socket, &_ev_server) == -1)
	{
		std::cerr << REDB <<  "Error:\n epoll_ctl server socket could not be set to monitored file descriptor list. Port "<< port << RESET << std::endl;
		std::cerr << "Reason: " << strerror(errno) << std::endl;
		throw ServerCriticalError();
	}
	return (server_socket);

}

ServerOS::~ServerOS(void)
{
    // create an map iterator element
	std::map<int, Connection *>::iterator it;

	// clean up any still open file descriptors or Connections
	for (it = _fd2client_map.begin(); it != _fd2client_map.end(); it++)
	{
		int 		fd 	 = it->first;
		Connection *conn = it->second;

		// // close file descriptor saved in the key
		close(fd);
		// // free allocated Connection
		delete conn;
		// remove entry from map
		// _fd2client_map.erase(fd);
		// it++; // testing
		// fd = -1;
	}
	_fd2client_map.clear();

	// close _epoll_fd
	close(_epoll_fd);
	_epoll_fd = -1;

	// close all sockets fds

	for (int i = 0; i < _nb_of_servers; i++)
	{
		close_server_socket(_servers_fd[i]);
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
		std::cerr << REDB <<  "Error:\n epoll_ctl connection fd could not be removed from monitored file descriptor list, fd: " << _epoll_fd << RESET << std::endl;
		std::cerr << strerror(errno) << std::endl;
		throw ServerCriticalError();
	}
	// delete allocated Connection
	if (VERBOSE == 1)
		std::cout << "file descriptor Connection object free  " << fd_to_remove << std::endl;
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

// check if the passed file descriptor belongs to a server socket
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

bool	ServerOS::_is_key_in_map(std::map<int, Connection *> *map, int key)
{
	if (map->find(key) == map->end())
		return (false);
	return (true);
}

/*
	EPOLLIN: Event file descriptor is available to read
	EPOLLOUT: Event file descriptor is available to write
	EPOLLERR: Event file descriptor error
	EPOLLHUP: Event file descriptor hang up
	EPOLLDHRHUP: Event file descriptor stream socket peer closed connection
*/

void	ServerOS::_loop(void)
{
	// NON BLOCKING - FOR LINUX OS Only

	if (_env == 0)
		std::cout << "Error env" << std::endl;

	int	MAX_EVENTS = 10; // Q:how to define this?
	int	nb_of_events = 0;
	int	client_fd = 0;
	int	error_counter = 0;


	// create address sizes structures
	socklen_t				client_addr_size;
	struct	sockaddr_in		client_addr;

	client_addr_size = sizeof(client_addr);

	// create an events structure for the events to be monitored
	struct epoll_event ep_event[MAX_EVENTS];

	// main loop
	while (1)
	{
		//epoll_wait with timeout equal -1 is infinite
		nb_of_events = epoll_wait(_epoll_fd, ep_event, MAX_EVENTS, -1);
		if (nb_of_events == -1)
		{
			print_error("epoll_wait");
			error_counter++;
			if (error_counter == 100)
			{
				print_error("Max amount of epoll_wait errors (100) reached. Application will be terminated");
				throw ServerCriticalError();
			}
		}
		for (int i = 0; i < nb_of_events; i++)
		{
			// print event triggered and file descriptor
			if (VERBOSE == 2)
				_print_epoll_events(ep_event[i].events, ep_event[i].data.fd);

			if (ep_event[i].events & EPOLLERR)
			{
				print_error_fd("Event Error received in fd ", ep_event[i].data.fd);
				_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server);
				print_error("Connection closed. Please retry");
			}

			else if (ep_event[i].events & EPOLLIN)
			{
				if (_is_a_server_socket(ep_event[i].data.fd) == true) // Request for a new connection
				{
					client_fd = accept(ep_event[i].data.fd, (struct sockaddr*)&client_addr, &client_addr_size);
					if (client_fd == -1)
					{
						print_error("accept. Connection refused");
						continue;
					}
					// check for IP4 or IP6
					if (client_addr.sin_family != AF_INET)
					{
						print_error_fd("Unsupport Connection socket Family. Only AF_INET (IPv4) is supported", client_fd);
						_close_connection(_epoll_fd, client_fd, _ev_server);
						continue;
					}

					// set client_fd to be reusablein a closed fd to allow fast reusable socket without waiting for the OS to release the ressource
					int yes = 1;
					if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
					{
						print_error_fd("setsockopt to set fd to SO_REUSEADDR to reusable. fd", client_fd);
						_close_connection(_epoll_fd, client_fd, _ev_server);
						print_error("Connection closed. Please retry");
						continue;
					}
					// set connection socket to nonblock too
					int flags = fcntl(client_fd, F_GETFL, 0, 0); // TODO is illegal function
					fcntl(client_fd, F_SETFL, flags | O_NONBLOCK); // TODO is illegal function

					// add file descriptor to be monitored
					_ev_server.data.fd = client_fd;

					// add new Connection file descriptor to the list of monitored file descriptors
					if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_ev_server) == -1)
					{
						print_error_fd("epoll_ctl. Not possible to add client to monitored events lists. fd", client_fd);
						_close_connection(_epoll_fd, client_fd, _ev_server);
						print_error("Connection closed. Please retry");
						continue;
					}

					// save fd as key and connection as a pointer to allow multiple clients at the same time and a expandable list/dictionary

					_fd2client_map[client_fd] = new Connection(_configParser ,client_fd, client_addr, ep_event[i].data.fd,  _servers_fd, _env);
					std::cout << std::endl << "New client connected in fd " << client_fd << std::endl;
					std::cout << "Client IP: " << _fd2client_map[client_fd]->get_client_ip() << std::endl;
					std::cout << "Client PORT: " << _fd2client_map[client_fd]->get_client_port() << std::endl;
				}
				else // file descriptor ready to read
				{
					bool is_cgi = false;
					char buffer[MAXMSG];
					int size_data_recv = 0;
					// int fd_read = -1;

					clear_memory(buffer, MAXMSG);
			
					if (_is_key_in_map (&_fd2client_map, ep_event[i].data.fd))
					{
						is_cgi = false;
					}
					else
					{
						is_cgi = true;
					}
					
					size_data_recv = read(ep_event[i].data.fd , buffer, MAXMSG);

					if (size_data_recv == -1 || size_data_recv == 0)
					{
						std::cout << REDB << "Error to recv : size of received is " << size_data_recv << RESET << std::endl;
						// std::cout<< REDB << "Error reason : " <<strerror(errno) << std::endl; // TODO remove
						if (is_cgi == true)
						{
							std::cout << REDB << "Closing file descriptor pipe read " << ep_event[i].data.fd << RESET << std::endl;
							close(ep_event[i].data.fd);
						}	
						else
						{
							std::cout << REDB << "Closing connection sockets " << ep_event[i].data.fd << " and returning" << RESET << std::endl;
							_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server);
						}
						continue;
					}

					if (is_cgi == true) // read from cgi socket
					{
						int connection_fd = _cgi_fd_2_connection_fd[ep_event[i].data.fd];
						_fd2client_map[connection_fd]->process_cgi(buffer, size_data_recv);

						if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, ep_event[i].data.fd, &_ev_server) == -1)
						{
							std::cerr << REDB <<  "Error:\n epoll_ctl pipe read fd could not be removed from file descriptor list" << RESET << std::endl;
							throw ServerCriticalError();
						}
						_cgi_fd_2_connection_fd.erase(connection_fd); // processed - remove from map
						close(ep_event[i].data.fd); // pipe fd 0
						_fd2client_map[connection_fd]->set_is_read_complete(true);
						continue;
					}
					else
					{
						// process read from socket
						_fd2client_map[ep_event[i].data.fd]->parse_request(buffer, size_data_recv);

						if (VERBOSE == 1)
							_fd2client_map[ep_event[i].data.fd]->print_request();
						

						if (_fd2client_map[ep_event[i].data.fd]->get_error() != 200)
						{
							std::cerr << "Error in Request-  error page: " << _fd2client_map[ep_event[i].data.fd]->get_error() << std::endl;
							print_error("Error: Connection closed. Please retry");
							_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server);
						}
						else
						{

							_fd2client_map[ep_event[i].data.fd]->create_response();

							if (_fd2client_map[ep_event[i].data.fd]->response_is_cgi())
							{

								int pipe_fd_0 = _fd2client_map[ep_event[i].data.fd]->get_fd_pipe_0();
								// if the response needs cgi, store the read pipe_fd 0
								_cgi_fd_2_connection_fd[pipe_fd_0] = ep_event[i].data.fd;
								// set connection socket to nonblock too

								//add it to the file descriptors to NONBLOCK  and set it to listened fd
								int flags = fcntl(pipe_fd_0, F_GETFL, 0, 0); // TODO is illegal function
								fcntl(pipe_fd_0, F_SETFL, flags | O_NONBLOCK); // TODO is illegal function

								// add file descriptor to be monitored
								_ev_server.data.fd = pipe_fd_0;

								// add new Connection file descriptor to the list of monitored file descriptors
								if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, pipe_fd_0, &_ev_server) == -1)
								{
									print_error_fd("epoll_ctl. Not possible to add pipe read fd to monitored events lists. fd", pipe_fd_0);
									std::cout << REDB << "errno " << strerror(errno) << RESET << std::endl;
									_close_connection(_epoll_fd, pipe_fd_0, _ev_server);
									print_error("Connection closed. Please retry");
									continue;
								}

							}	
							else
								_fd2client_map[ep_event[i].data.fd]->set_is_read_complete(true);

						}
							
					}
				}
			}
			// send message from client if read ready

			else if (ep_event[i].events & EPOLLOUT && _is_a_server_socket(ep_event[i].data.fd) == false && _fd2client_map[ep_event[i].data.fd]->get_is_read_complete())
			{
				if(_fd2client_map.find(ep_event[i].data.fd) != _fd2client_map.end())
				{
					try
					{

						_fd2client_map[ep_event[i].data.fd]->send_response();

					}
					catch(const ResponseError& e)
					{
						std::cout << e.what() << std::endl;
						_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server);
						continue;
					}
					catch(const EmptyResponseException& e)
					{
						std::cout << e.what() << std::endl;
						_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server);
						continue;
					}

					catch(const std::exception& e)
					{
						std::cerr << "Generic Exception during send_response" << '\n';
						std::cerr << e.what() << '\n';
						_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server); 
						continue;
					}

					// if keep alive or not: if not remove connection
					// if (_fd2client_map[ep_event[i].data.fd]->get_connection().compare("keep-alive") != 0)
					// {
						std::cout << "closing connection on fd " <<  ep_event[i].data.fd << std::endl; //TODO
						_fd2client_map[ep_event[i].data.fd]->set_is_read_complete(0); // prepare for next request
						_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server); // TODO remove
					// }
				}
				else
				{
					print_error_fd("Fd does not exist in _fd2client_map. fd", ep_event[i].data.fd);
					continue; // can we continue after this?
				}
			}
			else if (ep_event[i].events & EPOLLRDHUP)
			{
				//	EPOLLDHRHUP: Event file descriptor stream socket peer closed connection
				std::cout << "EPOLLRDHUP received for fd" << ep_event[i].data.fd << std::endl;
				_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server);
			}

			else if (ep_event[i].events & EPOLLHUP)
			{
				// EPOLLHUP: Event file descriptor hang up
				std::cout << "EPOLLHUP received for fd" << ep_event[i].data.fd << std::endl;
				_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server);
			}
		}

	}
}

