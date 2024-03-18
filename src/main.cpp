/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/18 21:47:09 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

// TODO Chunk extensions and passing key,values within
// TODO File Transfer with multipart/form-data
// TODO check Response code from Arafa and help
// TODO Ask tatiana to keep the interfaces as passed her
// TODO Ask tatiana to add this values
/*
	'# Set the number of worker processes to spawn
	Nginx_spawn 10;
	# Set the maximum number of connections each worker process can handle
	worker_connections 11;
	# Set the maximum script execution time
	script_timeout 2;
	# Set the maximum time the server will wait for a client request
	client_timeout 10;
	# Set the TCP backlog queue size to 10
	backlog 10;
	# Set the buffer size for reading client requests on bytes
	buff_size 40000;'''
*/
// TODO Ask Tatiana to add the server_default flag recognition as y or n
// TODO tatiana file upload front end
// TODO Talk to Arafa about the mime.types
// TODO Ask tatiana to add 411 page LengthRequiredException
// INFO risk: if python does not find the file it hangs (with Maira MVP for response) 

// TODO read body size from server block and use it in the server
// TODO check for no errno read in after a read/write
// TODO Server with no server name defined as default for servers with the same port
// TODO check the requirement in subject for : all read and write inside the loop
// TODO check other mandatory checks for request
// TODO setup nginx in a docker to compare behavior -> push it so others can also use it
// TODO what should happen if there is more than one server with default_server flag for the same port in the config file
// DONE Checked which methods can have a body. DELETE should not have a body and it will be ignored
// DONE check with content lenght mandatory
// DONE check if key case insensitive 
// DONE check if values are case insensitive and if so, apply lower case to all before comparing to anythign. (always compare with lower case)
// DONE Deactivateed Error in Port in request does not match socket port, should be active?

// Interface to Parser -> Server

/*
	run it by calling ./webserver configs/default.conf
*/

int	main(int argc, char *argv[], char *env[])
{
	std::string path_configfile;
	// if (argc != 2)
	// {
	// 	path_configfile = "config/default,conf";
	// }
	// else if (argc == 2 && strcmp(argv[2],"-h") == 0 || strcmp(argv[2],"--help") == 0))
	// {
	// 	sd::cout << REDB << "Please pass a configuration file to the webserver.\nFor example: ./webserver configs/single.conf" << std::endl;
	// 	print_error_error_exit("or run without arguments to use config/default -> ./webserver", 1);
	// }
	// else
	// {
	// 	path_configfile = argv[1];
	// }
	// consume
	if (argv[0] == 0 || argc == 0)
		return (1);

	std::cout << RESET;

 	ConfigParser configParser;
	// try {
	// 	configParser.getConfig(argv[1]);
	// } catch (const std::invalid_argument& e) {
	// 	std::cerr << "webserver Parser: " << e.what() << std::endl;
	// }


	//setup signal handler
	ft_setup_sighandler();

	try
	{
		ServerOS srv(0, configParser, env); // constructor setup socket and put it in listening mode
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