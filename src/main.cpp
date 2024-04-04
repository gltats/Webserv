/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/04 16:39:52 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

/*
	run it by calling ./webserver configs/default.conf
*/

int	main(int argc, char *argv[], char *env[])
{
    std::string		config;
    ConfigParser configParser;
	
	if(argc == 1 || argc == 2)
    {
        config = (argc == 1 ? "configs/default.conf" : argv[1]);
       	try 
		{
	    	configParser.getConfig(config);
		}
		catch (const std::invalid_argument& e)
		{
			std::cerr << REDB << "webserver Parser: " << e.what() << RESET << std::endl;
		}
		catch (const std::runtime_error& e)
		{
			std::cerr << REDB << "webserver Parser: " << e.what() << RESET << std::endl;
		}
    }
    else
    {
		std::cout << REDB << "Wrong arguments" << RESET << std::endl;
		exit(1);
	}   

	// configParser.setSize(2); // TODO: ATTENTION: remove this once nb of servers is available or setSize for the number of servers passed in default.conf
	
	if (configParser.getNumServers() == 0)
	{
		std::cerr << REDB << "No valid Server Configurations passed to the webserver" << RESET << std::endl;
		exit(1);
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