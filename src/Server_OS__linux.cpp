/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 16:31:13 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/06 22:47:18 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_OS__linux.hpp"

ServerOS::ServerOS(int server_index, ConfigParser &configParser, char *env[]): Server(server_index, configParser, env)
{
	
}

void	ServerOS::launch_webserver(void)
{

	_setup_socket();
	std::cout << "Socket Setup done" << std::endl;
	_listen_socket();
	std::cout << "Socket in listening mode port " << _server_port << std::endl;
	_loop();
}

void	ServerOS::_setup_socket(void)
{
	struct sockaddr_in serverAddr;
    
    // Socket will be created for AF_INET(IPv4), for SOCK_STREAM(TCP) and specific for the TCP protocol
	_server_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (_server_socket == -1)
	{   
        print_error("to create Server Socket");
		throw ServerCriticalError();
	}
	std::cout << "Socket created sucessfully. Socket fd = " << _server_socket << std::endl;

    // set socket option to reuse addr otherwise a time must be waited after closing a file descriptor until it can be reused
	int yes = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
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
	serverAddr.sin_port = htons(str2int(_server_port));

    // Bind socket to configuration defined in struct serverAddr
	if (bind(_server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
        print_error("to Bind socket and server_sockaddr");
		print_error(strerror(errno));
		throw ServerCriticalError();
	}
	std::cout << "Binding sucessful" << std::endl;
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
		_fd2client_map.erase(fd);
		fd = -1;
	}
	_fd2client_map.clear();

	// close _epoll_fd
	close(_epoll_fd);
	_epoll_fd = -1;
	// close socket fd
	close_server_socket();
}

void	ServerOS::_close_connection(int _epoll_fd, int fd_to_remove, struct epoll_event &ev_ref)
{
	if (VERBOSE == 1)
		std::cout << "Remove Connection fd = " << fd_to_remove << std::endl;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd_to_remove, &ev_ref) == -1)
	{
		std::cerr << REDB <<  "Error:\n epoll_ctl server socket could not be set to monitored file descriptor list" << RESET << std::endl;
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
		std::cout << "file descriptor removed from map " << fd_to_remove << std::endl;

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

	// create an epoll instance in a file descriptor
	int _epoll_fd = epoll_create(1);	 // argument is obsolete and must be >0

	// create an events structure for the events to be monitored
	struct epoll_event ev_server, ep_event[MAX_EVENTS];
	clear_memory((void *)&ev_server, sizeof(ev_server));

	// add file descriptor to be monitored
	ev_server.data.fd = _server_socket;
	// set events to be monitored in the file descriptor
	ev_server.events =  EPOLLIN | EPOLLOUT;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_socket, &ev_server) == -1)
	{
		std::cerr << REDB <<  "Error:\n epoll_ctl server socket could not be set to monitored file descriptor list" << RESET << std::endl;
		throw ServerCriticalError();
	}
	if (VERBOSE == 1)
		std::cout << "Server socket fd " << _server_socket << std::endl;
	
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
			/* print event triggered and file descriptor*/
			if (VERBOSE == 2)
				_print_epoll_events(ep_event[i].events, ep_event[i].data.fd);
		
			if (ep_event[i].events & EPOLLERR)
			{
				print_error_fd("Event Error received in fd ", ep_event[i].data.fd);
				_close_connection(_epoll_fd, ep_event[i].data.fd, ev_server);
				print_error("Connection closed. Please retry");
			}

			else if (ep_event[i].events & EPOLLIN)
			{	
				if (ep_event[i].data.fd == _server_socket) // Request for a new connection
				{
					client_fd = accept(_server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
					if (client_fd == -1)
					{
						print_error("accept. Connection refused");
						continue;
					}
					// check for IP4 or IP6
					if (client_addr.sin_family != AF_INET)
					{
						print_error_fd("Unsupport Connection socket Family. Only AF_INET (IPv4) is supported", client_fd);
						_close_connection(_epoll_fd, client_fd, ev_server);
						continue;
					}

					// set client_fd to be reusablein a closed fd to allow fast reusable socket without waiting for the OS to release the ressource
					int yes = 1;
					if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
					{
						print_error_fd("setsockopt to set fd to SO_REUSEADDR to reusable. fd", client_fd);
						_close_connection(_epoll_fd, client_fd, ev_server);
						print_error("Connection closed. Please retry");
						continue;
					}
					// set connection socket to nonblock too
					int flags = fcntl(client_fd, F_GETFL, 0, 0); // remove - illegal function
					fcntl(client_fd, F_SETFL, flags | O_NONBLOCK); // remove - illegal function

					// add file descriptor to be monitored
					ev_server.data.fd = client_fd;

					// add new Connection file descriptor to the list of monitored file descriptors
					if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev_server) == -1)
					{
						print_error_fd("epoll_ctl. Not possible to add client to monitored events lists. fd", client_fd);
						_close_connection(_epoll_fd, client_fd, ev_server);
						print_error("Connection closed. Please retry");
						continue;
					}
					
					// save fd as key and connection as a pointer to allow multiple clients at the same time and a expandable list/dictionary
					_fd2client_map[client_fd] = new Connection(_server_index, _configParser ,client_fd, client_addr, _env); 
					std::cout << "New client connected in fd " << client_fd << std::endl;
					std::cout << "Client IP: " << _fd2client_map[client_fd]->get_client_ip() << std::endl;
					std::cout << "Client PORT: " << _fd2client_map[client_fd]->get_client_port() << std::endl;
				}
				else // Request is from a already connected client
				{
					try
					{
						_fd2client_map[ep_event[i].data.fd]->receive_request();
					}				
					catch(const InvalidRequest& e)
					{
						std::cout << e.what() << std::endl;
					}
					catch(const MethodNotSupported& e)
					{
						std::cout << e.what() << std::endl;
					}
					catch(const std::exception& e)
					{
						std::cerr << "Generic Exception during receive_request" << '\n';
						std::cerr << e.what() << '\n';
					}
				}
			}
			// send message from client if read ready
			else if (ep_event[i].events & EPOLLOUT && ep_event[i].data.fd != _server_socket && _fd2client_map[ep_event[i].data.fd]->get_is_read_complete())
			{
				if (_fd2client_map[ep_event[i].data.fd])
				{
					try
					{
						_fd2client_map[ep_event[i].data.fd]->send_response();
					}
					catch(const ResponseError& e)
					{
						std::cout << e.what() << std::endl;
					}
					
					catch(const std::exception& e)
					{
						std::cerr << "Generic Exception during send_response" << '\n';
						std::cerr << e.what() << '\n';
					}

					// if keep alive or not: if not remove connection
					// if (_fd2client_map[ep_event[i].data.fd]->get_connection().compare("keep-alive") != 0)
					// {
						_close_connection(_epoll_fd, ep_event[i].data.fd, ev_server);
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
				_close_connection(_epoll_fd, ep_event[i].data.fd, ev_server);
			}
			
			else if (ep_event[i].events & EPOLLHUP)
			{
				// EPOLLHUP: Event file descriptor hang up
				_close_connection(_epoll_fd, ep_event[i].data.fd, ev_server);
			}
		}

	}
}

