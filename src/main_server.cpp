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
// void	launch_webserver_mac_os(std::map<std::string, std::string> &config_map, char *env[]);
// void	launch_webserver_linux_os(std::map<std::string, std::string> &config_map, char *env[]);

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
		launch_webserver(config_map, env);
		// if (OS == LINUX)
		// 	launch_webserver_linux_os(config_map, env);
		// else
		// 	launch_webserver_mac_os(config_map, env);

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

// void	launch_webserver_linux_os(std::map<std::string, std::string> &config_map, char *env[])
// {
// 	// NON BLOCKING - FOR LINUX OS Only
// 	(void)config_map;
// 	if (env == 0)
// 		std::cout << "Error env" << std::endl;
// }


// void	launch_webserver_mac_os(std::map<std::string, std::string> &config_map, char *env[])
// {
// 	// NON BLOCKING - FOR MAC OS Only
// 	socklen_t				client_addr_size;
// 	struct sockaddr_un		client_addr;

// 	client_addr_size = sizeof(client_addr);

// 	Server srv(config_map);

// 	int kq;
// 	struct kevent evSet; // Set of events we want to monitor


// 	kq = kqueue();
// 	if (kq == -1) {
// 		std::cout << "Error\nkqueue " << "Exiting" << std::endl;
// 		exit(1);
// 	}

// 	EV_SET(&evSet, srv.get_server_socket(), EVFILT_READ, EV_ADD, 0, 0, NULL); // macro that helps set the struct - are we allowed to use?
// 	kevent(kq, &evSet, 1, NULL, 0, NULL);

// 	int MAX_EVENTS= 200; // should this be the max backlog + 1 (server_fd)?
// 	struct kevent evList[MAX_EVENTS]; // to store event that happened -> must have a increaseable size and increase at every new connection dynamically
// 	int client_fd;
// 	struct kevent client_ev;

// 	// managing clients and responses content : file descriptor number is key
// 	std::map<int, Connection *> fd2client_map;
// 	int yes = 1;
// 	std::cout << "server fd " << srv.get_server_socket() << std::endl;
// 	while (1)
// 	{
// 		std::cout << "Server Listening in IP: LocalHost and Port: " << srv.get_server_port() << " ..." << std::endl;
// 		// Connection * client;
// 		int num_events = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
// 		std::cout << "number of events from kevent " << num_events << std::endl;

// 		for (int i = 0; i < num_events; i++)
// 		{
// 			if (evList[i].flags & EV_ERROR)
// 			{
//                 std::cerr << "Error in event" << std::endl;
//                 break;
// 			}
// 			else if (evList[i].ident == srv.get_server_socket() && evList[i].filter == EVFILT_READ) // server socket fd has received a new client connection request
// 			{
// 				client_fd = accept(srv.get_server_socket(), (struct sockaddr*)&client_addr, &client_addr_size);
// 				if (client_fd == -1)
// 				{
// 					std::cerr << "Connection refused for connection socket" << client_fd << std::endl; // get client ip and port would be nice
// 					// Handle error or non-blocking mode
// 					return ; // at the moment just return: error handling needs a lot of change here!
// 				}
// 				if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
// 				{
// 					std::cerr << "error to set fd " << client_fd << " to reusable" << std::endl;
// 				}
// 				// set client socket to nonblocking
// 				int flags = fcntl(client_fd, F_GETFL, 0);
//                 fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

// 				// add client socket to read and write event set
// 				EV_SET(&client_ev, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL); // macro that helps set the struct - are we allowed to use?
// 				kevent(kq, &client_ev, 1, NULL, 0, NULL); // set new event monitor to use the new

// 				fd2client_map[client_fd] = new Connection(client_fd, env); // connection client created -> needs to be an array or a data that increases in size to add more Connection objects
// 				std::cout << "New client connected " << client_fd << std::endl;
// 			}
// 			// client hang up and disconnected
//             else if (evList[i].flags & EV_EOF)
// 			{
//                 std::cerr << REDB << "Client disconnected " << RESET << std::endl; // get client ip and port would be nice
//                 EV_SET(&client_ev, evList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL); // remove client from event list
//              	kevent(kq, &client_ev, 1, NULL, 0, NULL);

// 			    EV_SET(&client_ev, evList[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL); // remove client from event list
// 			    kevent(kq, &client_ev, 1, NULL, 0, NULL);

//                 close(evList[i].ident);
// 				delete  fd2client_map[evList[i].ident];
// 				fd2client_map.erase(evList[i].ident); // remove from map
//             }
// 			// read message from client if read ready
//             else if (evList[i].filter == EVFILT_READ) // evList[i].ident == client_fd temporarily as only one client is supported at the time
// 			{
// 				std::cerr << YELLOW << "Event read fd "<< evList[i].ident << RESET << std::endl;
//                 fd2client_map[evList[i].ident]->receive_msg();
// 				EV_SET(&client_ev, evList[i].ident , EVFILT_READ, EV_DELETE, 0, 0, NULL); // remove client from event list
// 				kevent(kq, &client_ev, 1, NULL, 0, NULL);
// 				EV_SET(&client_ev, evList[i].ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL); // macro that helps set the struct - are we allowed to use?
// 				kevent(kq, &client_ev, 1, NULL, 0, NULL);
//             }
// 			// send message from client if read ready
//         	else if (evList[i].filter == EVFILT_WRITE )// evList[i].ident == client_fd temporarily as only one client is supported at the time
// 			{
// 				std::cerr << CYAN << "Event write fd "<< evList[i].ident << RESET << std::endl;
//                 fd2client_map[evList[i].ident]->send_response();

// 				EV_SET(&client_ev,evList[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL); // remove client from event list
//                 kevent(kq, &client_ev, 1, NULL, 0, NULL);

// 				close(evList[i].ident);
// 				delete  fd2client_map[evList[i].ident];
// 				fd2client_map.erase(evList[i].ident); // remove from map
// 				// remove client should be a function
//             }
// 			std::cout << std::endl << std::endl;
// 		}
// 	}

// }


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



