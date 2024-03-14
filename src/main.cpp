/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/14 15:43:46 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"


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