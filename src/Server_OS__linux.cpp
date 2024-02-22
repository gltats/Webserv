/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 16:31:13 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/22 18:30:06 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_OS__linux.hpp"

Server__OS::Server__OS(std::map<std::string, std::string> &config_map): Server(config_map)
{
	
}

void	Server__OS::setup_socket(void)
{
	struct sockaddr_in serverAddr;
    
    // Socket will be created for AF_INET(IPv4), for SOCK_STREAM(TCP) and specific for the TCP protocol
	_server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_socket == -1)
	{   
        print_error("to create Server Socket");
		exit (1);
	}
	std::cout << "Socket created sucessfully. Socket fd = " << _server_socket << std::endl;

    // set socket option to reuse addr otherwise a time must be waited after closing a file descriptor until it can be reused
	int yes = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
        print_error("setsockopt to SO_REUSEADDR");
		close(_server_socket);
		exit (1);
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
		close(_server_socket);
		exit (1);
	}
	std::cout << "Binding sucessful" << std::endl;
}

Server__OS::~Server__OS(void)
{
    
}
