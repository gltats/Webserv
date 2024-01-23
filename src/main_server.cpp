/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/01/23 20:54:54 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

void	launch_webserver(std::map<std::string, std::string> &config_map, std::map<std::string, std::string> &error_page_map, char *env[]);

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

	// map default error pages number and path
	std::map<std::string, std::string> error_page_map;
	map_default_error_pages(error_page_map);

	//setup signal handler
	ft_setup_sighandler();

	// launch webserver loop
	launch_webserver(config_map, error_page_map, env);

	return (0);
}

void	launch_webserver(std::map<std::string, std::string> &config_map, std::map<std::string, std::string> &error_page_map, char *env[])
{
	socklen_t				client_addr_size;
	struct sockaddr_un		client_addr;

	client_addr_size = sizeof(client_addr);

	Server srv(config_map, error_page_map);
	while (true) // correct infinite loop
	// bool sw = 0;
	// while (sw == 0) // testing not infinite loop
	{
		std::cout << "Server Listening in IP: LocalHost and Port: " << srv.get_server_port() << " ..." << std::endl;

		int new_socket = accept(srv.get_server_socket(), (struct sockaddr*)&client_addr, &client_addr_size);
		if (new_socket == -1)
		{
			// Handle error or non-blocking mode
			return ; // at the moment just return
		}

		Connection	client(new_socket, env, error_page_map);

		client.receive_msg();
		client.send_response();
		close(new_socket);

		// sw = 1;

	}
		srv.close_socket(); // temporary
}


