/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/09 22:16:17 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"


void	launch_webserver(std::map<std::string, std::string> &config_map, char *env[]);
void	launch_webserver_mac_os(std::map<std::string, std::string> &config_map, char *env[]);
void	launch_webserver_linux_os(std::map<std::string, std::string> &config_map, char *env[]);

// Interface to Parser -> Server
// Tatiana please adapt map_config_file function in file map.cpp

int	main(int argc, char *argv[], char *env[])
{
	// to eliminate unused arguments error
	if (argc == 0 || argv == 0 || env == 0)
		return (1);

	//--> Edit Here :here goes the parser Config file (being simulated at the moment <--
	ConfigParser parser_output;
	// Stop editing

	// map parsed data from config_file (Interface) to be used in Server Object
	std::map<std::string, std::string> config_map;
	map_config_file(config_map, parser_output); // <- Tatiana: use this function to map you parser_out to the Server map inputs


	//setup signal handler
	ft_setup_sighandler();

	// launch webserver loop
	try
	{
		// launch_webserver(config_map, env);
		// if (OS == LINUX)
			launch_webserver_linux_os(config_map, env);
		// else
			// launch_webserver_mac_os(config_map, env);

	}
	catch(const UserRequestTermination& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}


void	close_connection_linux_os(int epoll_fd, int fd_to_remove, struct epoll_event &ev_ref, std::map<int, Connection *> &fd2client_map_ref, Server &srv)
{
	if (DEBUG == 1)
		std::cout << "Remove Connection fd = " << fd_to_remove << std::endl;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd_to_remove, &ev_ref) == -1)
	{
		std::cerr << REDB <<  "Error:\n epoll_ctl server socket could not be set to monitored file descriptor list" << RESET << std::endl;
		// throw exception
		// close socket file descritpro
		srv.close_server_socket();
		exit(1); // at the moment
	}
	// delete allocated Connection
	std::cout << "file descriptor Connection object freeid  " << fd_to_remove << std::endl;

	delete  fd2client_map_ref[fd_to_remove];


	// close file descriptor
	std::cout << "file descriptor closed  " << fd_to_remove << std::endl;

	close(fd_to_remove);
	// remove from map
	std::cout << "file descriptor removed from map " << fd_to_remove << std::endl;

	fd2client_map_ref.erase(fd_to_remove); 
}

void	launch_webserver_linux_os(std::map<std::string, std::string> &config_map, char *env[])
{
	// NON BLOCKING - FOR LINUX OS Only
	(void)config_map;
	if (env == 0)
		std::cout << "Error env" << std::endl;

	int	MAX_EVENTS = 10; // Q:how to define this?
	int	nb_of_events = 0;
	int	client_fd = 0;

	// ignore sigpipe
	signal(SIGPIPE, SIG_IGN);

	// create address sizes structures
	socklen_t				client_addr_size;
	struct sockaddr_un		client_addr;

	client_addr_size = sizeof(client_addr);

	// create a Server instance
	Server srv(config_map); // constructor setup socket and put it in listening mode

	// create an epoll instance in a file descriptor
	int epoll_fd = epoll_create(1);	 // argument is obsolete and must be >0

	// create an events structure for the events to be monitored
	struct epoll_event ev_server, ev_clients, ep_event[MAX_EVENTS];
	memset(&ev_server, '\0', sizeof(ev_server));

	// add file descriptor to be monitored
	
		// EPOLLIN: Event file descriptor is available to read
		// EPOLLOUT: Event file descriptor is available to write
		// EPOLLERR: Event file descriptor error
		// EPOLLHUP: Event file descriptor hang up
		// EPOLLDHRHUP: Event file descriptor stream socket peer closed connection
	
	// ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
	// ev.events = EPOLLIN  | EPOLLOUT | EPOLLET;
	
	int server_event_flags = EPOLLIN | EPOLLET;
	int client_event_flags = EPOLLIN  | EPOLLOUT | EPOLLET;
	
	ev_server.events = server_event_flags;
	ev_server.data.fd = srv.get_server_socket();

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, srv.get_server_socket(), &ev_server) == -1)
	{
		std::cerr << REDB <<  "Error:\n epoll_ctl server socket could not be set to monitored file descriptor list" << RESET << std::endl;
		// throw exception
		// close socket file descritpro
		srv.close_server_socket();
		exit(1); // at the moment
	}

	ev_server.events = client_event_flags;

	// memset(&ev_clients, '\0', sizeof(ev_server));
	// ev_clients.events = EPOLLIN  | EPOLLOUT | EPOLLET;

	// map to hold Connection Object pointer per file descriptor
	std::map<int, Connection *> fd2client_map;

	if (DEBUG == 1)
		std::cout << std::endl << std::endl;

	std::cout << "server socket is " << srv.get_server_socket() << std::endl;
	// main loop 
	while (1)
	{
		//epoll_wait with timeout equal -1 is infinite
		nb_of_events = epoll_wait(epoll_fd, ep_event, MAX_EVENTS, -1);

		std::cout << "nb of events " << nb_of_events << std::endl; // remove

		for (int i = 0; i < nb_of_events; i++)
		{
			std::cout << "event on file descriptor " << ep_event[i].data.fd << std::endl;
			if (ep_event[i].events & EPOLLERR)
			{
           		std::cerr << REDB << "Event Error fd "<< ep_event[i].data.fd << RESET << std::endl;
				std::cerr << REDB << "errno " << strerror(errno) << RESET << std::endl; // for testing
				exit(1);
				// break;
			}

			else if (ep_event[i].events & EPOLLIN)
			{	
				if (ep_event[i].data.fd == srv.get_server_socket()) // Request for a new connection
				{
					if (DEBUG == 1)
						std::cerr << YELLOW << "Event read - New Client on server socket "<< ep_event[i].data.fd << RESET << std::endl;
					client_fd = accept(srv.get_server_socket(), (struct sockaddr*)&client_addr, &client_addr_size);
					if (client_fd == -1)
					{
	
						std::cerr << REDB << "Error accept, Connection refused for connection socket " << client_fd << RESET << std::endl;
						// Handle error or non-blocking mode
						// break ; // at the moment just return: error handling needs a lot of change here!
						exit(1); // test
					}

					// set client_fd to be reusablein a closed fd to allow fast reusable socket without waiting for the OS to release the ressource
					int yes = 1;
					if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
					{
						std::cerr << REDB << "Error setsockopt to set fd to SO_REUSEADDR " << client_fd << " to reusable" << std::endl;
						// handle error
						break; 
					}
					// add new Connection file descriptor to the list of monitored file descriptors
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev_server) == -1)
					{
						std::cerr << REDB <<  "Error epoll_ctl new client socket could not be set to monitored file descriptor list" << RESET << std::endl;
						close(client_fd);
						break; // at the moment
					}
					// set connection socket to nonblock too
					int flags = fcntl(client_fd, F_GETFL, 0, 0); // remove - illegal function
					int ret =  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);;

					// save fd as key and connection as a pointer to allow multiple clients at the same time and a expandable list/dictionary
					fd2client_map[client_fd] = new Connection(client_fd, env); 
					std::cout << "New client connected in fd " << client_fd << std::endl;

					//testing
					fd2client_map[client_fd]->receive_msg();

					// fd2client_map[client_fd]->send_response(); // for testing

					// freeaddrinfo((struct addrinfo *)&client_addr);
				}
				else // Request is from a already connected client
				{
					if (DEBUG == 1)
						std::cerr << YELLOW << "Event read data - fd "<< ep_event[i].data.fd << RESET << std::endl;
					fd2client_map[ep_event[i].data.fd]->receive_msg();
				}
			}
			// send message from client if read ready
			else if (ep_event[i].events & EPOLLOUT && ep_event[i].data.fd != srv.get_server_socket() && fd2client_map[ep_event[i].data.fd]->get_is_read_complete())
			{
				if (DEBUG == 1)
					std::cout << YELLOW << "Event write in  socket (fd = " << ep_event[i].data.fd << ")" << RESET<<  std::endl;

				if (fd2client_map.find(ep_event[i].events) == fd2client_map.end())
        		{
					std::cout << "Error: fd " <<  ep_event[i].data.fd << " does not exist in fd2client_map" << std::endl;
					continue;
				}

			

				// if (fd2client_map[ep_event[i].data.fd]->is_response_empty() == false)


					fd2client_map[ep_event[i].data.fd]->send_response();
				// if keep alive or not: if not remove connection
				// if (fd2client_map[ep_event[i].data.fd]->get_connection().compare("keep-alive") != 0)
				// {
				 	close_connection_linux_os(epoll_fd, ep_event[i].data.fd, ev_server, fd2client_map, srv);
				// }
			}
			// send message from client if read ready
			else if (ep_event[i].events & EPOLLRDHUP)
			{
				if (DEBUG == 1)
					std::cout << YELLOW << " EPOLLRDHUP - Event Stream socket peer(fd = " << ep_event[i].data.fd << ") closed connection" << RESET << std::endl;
				close_connection_linux_os(epoll_fd, ep_event[i].data.fd, ev_server, fd2client_map, srv);
			}
			
			else if (ep_event[i].events & EPOLLHUP)
			{
				if (DEBUG == 1)
					std::cout << YELLOW << "EPOLLHUP  - Event file descriptor(fd = " << ep_event[i].data.fd << ") hang up" << RESET << std::endl;
				close_connection_linux_os(epoll_fd, ep_event[i].data.fd, ev_server, fd2client_map, srv);
			}
		}
		if (DEBUG == 1)
			std::cout << std::endl << std::endl;

	}

	// // create an map iterator
	// std::map<int, Connection *>::iterator it;

	// // clean up any still open file descriptors or Connections
	// for (int it = fd2client_map.begin(); it != fd2client_map.end(); it++)
	// {
	// 	
	// 		fd 			= fd2client_map->first
	// 		*Connection = fd2client_map->second
	// 	
		
	// 	// close file descriptor saved in the key
	// 	close(fd2client_map->first);
	// 	// free allocated Connection
	// 	delete fd2client_map->second;
	// 	// remove entry from map
	// 	fd2client_map.erase(fd2client_map->first);

	// }
	// close epoll_fd
	close(epoll_fd);
	epoll_fd = -1;
	// close socket fd
	srv.close_server_socket();
}


/*
void	launch_webserver_mac_os(std::map<std::string, std::string> &config_map, char *env[])
{
	// NON BLOCKING - FOR MAC OS Only
	socklen_t				client_addr_size;
	struct sockaddr_un		client_addr;

	client_addr_size = sizeof(client_addr);

	Server srv(config_map);

	int kq;
	struct kevent evSet; // Set of events we want to monitor


	kq = kqueue();
	if (kq == -1) {
		std::cout << "Error\nkqueue " << "Exiting" << std::endl;
		exit(1);
	}

	EV_SET(&evSet, srv.get_server_socket(), EVFILT_READ, EV_ADD, 0, 0, NULL); // macro that helps set the struct - are we allowed to use?
	kevent(kq, &evSet, 1, NULL, 0, NULL);

	int MAX_EVENTS= 200; // should this be the max backlog + 1 (server_fd)?
	struct kevent evList[MAX_EVENTS]; // to store event that happened -> must have a increaseable size and increase at every new connection dynamically
	int client_fd;
	struct kevent client_ev;

	// managing clients and responses content : file descriptor number is key
	std::map<int, Connection *> fd2client_map;
	int yes = 1;
	std::cout << "server fd " << srv.get_server_socket() << std::endl;
	while (1)
	{
		std::cout << "Server Listening in IP: LocalHost and Port: " << srv.get_server_port() << " ..." << std::endl;
		// Connection * client;
		int num_events = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
		std::cout << "number of events from kevent " << num_events << std::endl;

		for (int i = 0; i < num_events; i++)
		{
			if (evList[i].flags & EV_ERROR)
			{
                std::cerr << "Error in event" << std::endl;
                break;
			}
			else if (evList[i].ident == srv.get_server_socket() && evList[i].filter == EVFILT_READ) // server socket fd has received a new client connection request
			{
				client_fd = accept(srv.get_server_socket(), (struct sockaddr*)&client_addr, &client_addr_size);
				if (client_fd == -1)
				{
					std::cerr << "Connection refused for connection socket" << client_fd << std::endl; // get client ip and port would be nice
					// Handle error or non-blocking mode
					return ; // at the moment just return: error handling needs a lot of change here!
				}
				if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
				{
					std::cerr << "error to set fd " << client_fd << " to reusable" << std::endl;
				}
				// set client socket to nonblocking
				int flags = fcntl(client_fd, F_GETFL, 0);
				std::cout << "socket flags for new connections standard is " << flags << std::endl;
                fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

				// add client socket to read and write event set
				EV_SET(&client_ev, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL); // macro that helps set the struct - are we allowed to use?
				kevent(kq, &client_ev, 1, NULL, 0, NULL); // set new event monitor to use the new

				fd2client_map[client_fd] = new Connection(client_fd, env); // connection client created -> needs to be an array or a data that increases in size to add more Connection objects
				std::cout << "New client connected " << client_fd << std::endl;
			}
			// client hang up and disconnected
            else if (evList[i].flags & EV_EOF)
			{
                std::cerr << REDB << "Client disconnected " << RESET << std::endl; // get client ip and port would be nice
                EV_SET(&client_ev, evList[i].ident , EVFILT_READ, EV_DELETE, 0, 0, NULL); // remove client from event list
             	kevent(kq, &client_ev, 1, NULL, 0, NULL);

			    EV_SET(&client_ev, evList[i].ident , EVFILT_WRITE, EV_DELETE, 0, 0, NULL); // remove client from event list
			    kevent(kq, &client_ev, 1, NULL, 0, NULL);

                close(evList[i].ident );
				delete  fd2client_map[evList[i].ident ];
				fd2client_map.erase(evList[i].ident ); // remove from map
            }
			// read message from client if read ready
            else if (evList[i].filter == EVFILT_READ) // ep_event[i].data.fd == client_fd temporarily as only one client is supported at the time
			{
				std::cerr << YELLOW << "Event read fd "<< evList[i].ident  << RESET << std::endl;
                fd2client_map[evList[i].ident ]->receive_msg();
				EV_SET(&client_ev, evList[i].ident  , EVFILT_READ, EV_DELETE, 0, 0, NULL); // remove client from event list
				kevent(kq, &client_ev, 1, NULL, 0, NULL);
				EV_SET(&client_ev,evList[i].ident , EVFILT_WRITE, EV_ADD, 0, 0, NULL); // macro that helps set the struct - are we allowed to use?
				kevent(kq, &client_ev, 1, NULL, 0, NULL);
            }
			// send message from client if read ready
        	else if (evList[i].filter == EVFILT_WRITE )// ep_event[i].data.fd == client_fd temporarily as only one client is supported at the time
			{
				std::cerr << CYAN << "Event write fd "<< evList[i].ident  << RESET << std::endl;
                fd2client_map[evList[i].ident ]->send_response();

				EV_SET(&client_ev, evList[i].ident  , EVFILT_WRITE, EV_DELETE, 0, 0, NULL); // remove client from event list
                kevent(kq, &client_ev, 1, NULL, 0, NULL);

				close(evList[i].ident );
				delete  fd2client_map[evList[i].ident ];
				fd2client_map.erase(evList[i].ident ); // remove from map
				// remove client should be a function
            }
			std::cout << std::endl << std::endl;
		}
	}

}
*/

/*
void	launch_webserver(std::map<std::string, std::string> &config_map, char *env[])
{
	socklen_t				client_addr_size;
	struct sockaddr_un		client_addr;

	// Blocking
	client_addr_size = sizeof(client_addr);

	Server srv(config_map);

	while (true)
	{
		std::cout << "Server Listening in IP: LocalHost and Port: " << srv.get_server_port() << " ..." << std::endl;

		int client_fd = accept(srv.get_server_socket(), (struct sockaddr*)&client_addr, &client_addr_size);
		if (client_fd == -1)
		{
			// Handle error or non-blocking mode
			return ; // at the moment just return
		}

		Connection	client(client_fd, env);

		client.receive_msg();
		client.send_response();
		close(client_fd);

	}
		srv.close_server_socket(); // temporary
}
*/


