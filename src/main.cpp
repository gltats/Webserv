/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/06 23:23:31 by mgranero         ###   ########.fr       */
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