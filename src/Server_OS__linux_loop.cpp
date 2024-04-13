/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux_loop.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 09:09:00 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 12:06:20 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_OS__linux.hpp"


/*
	epoll Events Meaning
	EPOLLIN: Event file descriptor is available to read
	EPOLLOUT: Event file descriptor is available to write
	EPOLLERR: Event file descriptor error
	EPOLLHUP: Event file descriptor hang up
	EPOLLDHRHUP: Event file descriptor stream socket peer closed connection
*/

void	ServerOS::launch_webserver_loop(void)
{
	int						nb_of_events = 0;
	struct epoll_event		ep_event[MAX_EVENTS]; // create an events structure for the events to be monitored

	while (1)
	{
		//epoll_wait with timeout equal -1 is waiting for events without a timeout
		nb_of_events = epoll_wait(_epoll_fd, ep_event, MAX_EVENTS, -1);
		if (nb_of_events == -1)
		{
			print_error("epoll_wait returned -1 error");
			throw ServerCriticalError();
		}
		for (int i = 0; i < nb_of_events; i++)
		{
			if (ep_event[i].events & EPOLLERR)
				_handle_epollerr(ep_event[i].data.fd);

			else if (ep_event[i].events & EPOLLIN)
			{
				if (_handle_epollin(ep_event[i].data.fd) == false)
					continue;
			}
			// send message from client if read ready

			else if (ep_event[i].events & EPOLLOUT && _is_a_server_socket(ep_event[i].data.fd) == false && _fd2client_map[ep_event[i].data.fd]->get_is_read_complete())
			{
				// TODO handle EPOLLOUT
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

					if (_fd2client_map[ep_event[i].data.fd]->get_error() != 200) 
					{
						_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server);
					}
					else
					{	
						// if keep alive or not: if not remove connection
						// if (_fd2client_map[ep_event[i].data.fd]->get_connection().compare("keep-alive") != 0)
						// {
							std::cout << "closing connection on fd as non persistent" <<  ep_event[i].data.fd << std::endl; //TODO
							_fd2client_map[ep_event[i].data.fd]->set_is_read_complete(false); // prepare for next request
							_close_connection(_epoll_fd, ep_event[i].data.fd, _ev_server); // TODO remove
					}
				}
				else
				{
					print_error_fd("Fd does not exist in _fd2client_map. fd", ep_event[i].data.fd);
					continue; // can we continue after this?
				}
			}
			else if (ep_event[i].events & EPOLLRDHUP)
				_handle_epollrdhup(ep_event[i].data.fd);

			else if (ep_event[i].events & EPOLLHUP)
				_handle_epollhup(ep_event[i].data.fd);
		}
    }
}

void	ServerOS::_handle_epollerr(int fd)
{
	print_error_fd("Event Error received in fd: ", fd);
	_close_connection(_epoll_fd, fd, _ev_server);
}

bool	ServerOS::_handle_epollin(int fd)
{
	// check if the fd ready to read is a server socket or a pipe
	if (_is_a_server_socket(fd) == true) 
	{
		// Request for a new connection
		return (_add_connection(fd));
	}
	else
	{
		// request is to read data from a pipe or already connected client
		return(_read_from_fd(fd));
	}
	return (true);
}

// returns false if an error occorred and loop must call "continue", otherwise returns true
bool	ServerOS::_add_connection(int socket_fd)
{
	int						client_fd = -1;
	socklen_t				client_addr_size = 0; 
	struct	sockaddr_in		client_addr;
	int 					yes = 1;

	client_addr_size = sizeof(client_addr);

	client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);
	if (client_fd == -1)
	{
		print_error_fd("Accept function failed. Connection refused", socket_fd);
		std::cerr << REDB << "Reason: " << strerror(errno) << RESET << std::endl;
		return (false);
	}
	// check for IP4 or IP6
	if (client_addr.sin_family != AF_INET)
	{
		print_error_fd("Unsupport Connection socket Family. Only AF_INET (IPv4) is supported", client_fd);
		_close_connection(_epoll_fd, client_fd, _ev_server);
		return (false);
	}

	// set client_fd to be reusable after closed without waiting for the OS to release the ressource
	if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		print_error_fd("setsockopt to set fd to SO_REUSEADDR to reusable. fd", client_fd);
		std::cerr << REDB << "Reason: " << strerror(errno) << RESET << std::endl;
		_close_connection(_epoll_fd, client_fd, _ev_server);
		return (false);
	}
	
	// set connection socket to nonblock too
	_configure_fd_nonblock(client_fd);

	// add new Connection file descriptor to the list of monitored file descriptors
	_add_fd_to_monitored_events(client_fd);

	// save fd as key and connection as a pointer to allow multiple clients at the same time and a expandable list/dictionary
	_fd2client_map[client_fd] = new Connection(_configParser ,client_fd, client_addr, socket_fd,  _servers_fd, _env);
	
	if (VERBOSE == 1)
	{
		std::cout << std::endl << "New client connected in fd " << client_fd << std::endl;
		std::cout << "Client IP: " << _fd2client_map[client_fd]->get_client_ip() << std::endl;
		std::cout << "Client PORT: " << _fd2client_map[client_fd]->get_client_port() << std::endl;
		std::cout << "Current number of connected Clients: " << Connection::get_nb_connections() << std::endl;
	}
	return (true);
}

// returns false if an error occorred and loop must call "continue", otherwise returns true
bool	ServerOS::_read_from_fd(int fd)
{
	bool is_cgi;
	char buffer[MAX_MSG_BUFFER];
	int size_data_recv = 0;

	clear_memory(buffer, MAX_MSG_BUFFER);

	// checks if file descriptor is a client fd or a pipe
	if (_is_key_in_map (&_fd2client_map, fd))
		is_cgi = false;
	else
		is_cgi = true;

	size_data_recv = read(fd , buffer, MAX_MSG_BUFFER);
	if (size_data_recv == -1 || size_data_recv == 0)
	{
		std::cout << REDB << "Error to recv : size of received is " << size_data_recv << ", in fd " << fd << RESET << std::endl;
		// std::cout<< REDB << "Error reason : " <<strerror(errno) << std::endl; // for testing TODO remove
		if (is_cgi == true)
		{
			if (VERBOSE == 1)
				std::cout << REDB << "Error to read from pipe fd"<< RESET << std::endl;
			_remove_fd_from_monitored_events(fd);
			close(fd);
		}
		else
		{
			if (VERBOSE == 1)
				std::cout << REDB << "Error to read from client fd"<< RESET << std::endl;
			_close_connection(_epoll_fd, fd, _ev_server);
		}
		return (false);
	}

	if (is_cgi == true) // process read from cgi pipe
	{
		int connection_fd; // retrieve connection fd associated with this pipe fd
		connection_fd = _cgi_fd_2_connection_fd[fd];
		_fd2client_map[connection_fd]->process_cgi(buffer, size_data_recv);
		_remove_fd_from_monitored_events(fd);
		// pipe processed - remove from map
		_cgi_fd_2_connection_fd.erase(connection_fd);
		close(fd);
		_fd2client_map[connection_fd]->set_is_read_complete(true);
		return (false);
	}
	else
	{
		// process read buffer from client connection
		_fd2client_map[fd]->parse_request(buffer, size_data_recv);

		if (VERBOSE == 1)
			_fd2client_map[fd]->print_request();

		_fd2client_map[fd]->create_response();
		// close connection if response status is different from 200

		if (_fd2client_map[fd]->response_is_cgi())
		{
			// the request trigger a cgi which need to be setup to be read once the pipe is ready to write
			int pipe_fd_0 = _fd2client_map[fd]->get_fd_pipe_0();
			// response needs to read from pipe returned from cgi, store the read pipe_fd 0
			_cgi_fd_2_connection_fd[pipe_fd_0] = fd;

			//add it to the file descriptors to NONBLOCK  and set it to listened fd
			_configure_fd_nonblock(pipe_fd_0);

			// add new Connection file descriptor to the list of monitored file descriptors
			_add_fd_to_monitored_events(pipe_fd_0);
		}
		else
			_fd2client_map[fd]->set_is_read_complete(true);

	}
	return (true);
}


void	ServerOS::_handle_epollrdhup(int fd)
{
	//	EPOLLDHRHUP: Event file descriptor stream socket peer closed connection
	if (VERBOSE == 1)
		std::cout << "EPOLLRDHUP received for fd" << fd << std::endl;
	_close_connection(_epoll_fd, fd, _ev_server);
}

void	ServerOS::_handle_epollhup(int fd)
{
	// EPOLLHUP: Event file descriptor hang up
	if (VERBOSE == 1)
		std::cout << "EPOLLHUP received for fd" << fd << std::endl;
	_close_connection(_epoll_fd,fd, _ev_server);
}
