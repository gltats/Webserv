/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/04 21:12:05 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

// # if defined (__APPLE__)

// # elif defined (__linux__)

void	launch_webserver(std::map<std::string, std::string> &config_map, char *env[]);

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

void	launch_webserver(std::map<std::string, std::string> &config_map, char *env[])
{
	socklen_t				client_addr_size;
	struct sockaddr_un		client_addr;

	client_addr_size = sizeof(client_addr);

	Server srv(config_map);

	while (true)
	{
		std::cout << "Server Listening in IP: LocalHost and Port: " << srv.get_server_port() << " ..." << std::endl;

		int new_socket = accept(srv.get_server_socket(), (struct sockaddr*)&client_addr, &client_addr_size);
		if (new_socket == -1)
		{
			// Handle error or non-blocking mode
			return ; // at the moment just return
		}

		Connection	client(new_socket, env);

		client.receive_msg();
		client.send_response();
		close(new_socket);

	}
		srv.close_server_socket(); // temporary
}



