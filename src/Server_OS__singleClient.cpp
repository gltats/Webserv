/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__singleClient.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 16:31:13 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/23 13:57:20 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_OS__singleClient.hpp"

ServerOS::ServerOS(int server_index, ConfigParser &configParser, char *env[]): Server(server_index, configParser, env)
{
	
}

void	ServerOS::launch_webserver(void)
{

	_setup_socket();
	std::cout << "Socket Setup done" << std::endl;
	_listen_socket();
	std::cout << "Socket in listening mode" << std::endl;
	_loop();
}

void	ServerOS::_setup_socket(void)
{
	struct sockaddr_in serverAddr;
	int		server_socket;
    
    // Socket will be created for AF_INET(IPv4), for SOCK_STREAM(TCP) and specific for the TCP protocol
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	serverAddr.sin_port = htons(str2int(_server_port));

    // Bind socket to configuration defined in struct serverAddr
	if (bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
        print_error("to Bind socket and server_sockaddr");
		print_error(strerror(errno));
		throw ServerCriticalError();
	}
	std::cout << "Binding sucessful" << std::endl;
	return (server_socket);
}

ServerOS::~ServerOS(void)
{
	// close socket fd
	close_server_socket();
}

void	ServerOS::_loop(void) //new generic
{
	// BLOCKING - FOR LINUX OS and MAC. One Client Only

	if (_env == 0)
		std::cout << "Error env" << std::endl;

	int	client_fd = 0;

	// create address sizes structures
	socklen_t				client_addr_size;
	struct	sockaddr_in		client_addr;

	client_addr_size = sizeof(client_addr);

	while (true)
	{
		std::cout << "Server Listening in IP: LocalHost and Port: " << _server_port << " ..." << std::endl;

		client_fd = accept(_server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
		if (client_fd == -1)
		{
			print_error("accept function.");
			print_error(strerror(errno));
			close(client_fd);
			continue;
		}

		Connection	client(_server_index, _configParser,client_fd, client_addr, _env); 

		std::cout << "New client connected in fd " << client_fd << std::endl;
		std::cout << "Client IP: " << client.get_client_ip() << std::endl;
		std::cout << "Client PORT: " << client.get_client_port() << std::endl;

		client.receive_request();
		client.send_response();

		close(client_fd);
		client_fd = -1;

	}
}
