/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux_socket.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 09:11:26 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:17:19 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_OS__linux.hpp"


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

void	ServerOS::_close_server_socket(int fd)
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
