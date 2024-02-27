/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/27 21:32:02 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"


// Interface to Parser -> Server

/*
	run it by calling ./webserver configs/default.conf
*/

int	main(int argc, char *argv[], char *env[])
{
	// to eliminate unused arguments error
	if (argc != 2)
		print_error_error_exit("Please pass a configuration file to the webserver.\nFor example: ./webserver configs/default.conf", 1);

 	ConfigParser configParser;
	try {
		configParser.getConfig(argv[1]);
	} catch (const std::invalid_argument& e) {
		std::cerr << "webserver Parser: " << e.what() << std::endl;
	}

	//setup signal handler
	ft_setup_sighandler();

	// launch webserver loop
	try
	{
		// create a Server instance
		ServerOS srv(configParser, env); // constructor setup socket and put it in listening mode

		srv.setup_socket();
		std::cout << "Socket Setup done" << std::endl;
		srv.listen_socket();
		std::cout << "Socket in listening mode" << std::endl;
		srv.launch_webserver();
	}
	catch(const UserRequestTermination& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(const ServerCriticalError& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
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




