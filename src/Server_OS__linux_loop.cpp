/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux_loop.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 09:09:00 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:24:02 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_OS__linux.hpp"


/*
	EPOLLIN: Event file descriptor is available to read
	EPOLLOUT: Event file descriptor is available to write
	EPOLLERR: Event file descriptor error
	EPOLLHUP: Event file descriptor hang up
	EPOLLDHRHUP: Event file descriptor stream socket peer closed connection
*/

void	ServerOS::launch_webserver_loop(void)
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
					std::cout << "Current number of connected Clients: " << Connection::get_nb_connections() << std::endl;

				}
				else // file descriptor ready to read
				{
					bool is_cgi = false;
					char buffer[MAX_MSG_BUFFER];
					int size_data_recv = 0;
					// int fd_read = -1;

					clear_memory(buffer, MAX_MSG_BUFFER);

					if (_is_key_in_map (&_fd2client_map, ep_event[i].data.fd))
					{
						is_cgi = false;
					}
					else
					{
						is_cgi = true;
					}

					size_data_recv = read(ep_event[i].data.fd , buffer, MAX_MSG_BUFFER);

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