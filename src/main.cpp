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

// Sofia's tests ignore
// void test()
// {
//     { 
//         // Test 1: pass
//         ConfigParser configParser;
//         try {
//             configParser.getConfig("configs/default.conf");
//             // std::cout << configParser.getParameterValue(1, "listen") << std::endl;
//             std::cout << configParser.getParameterValue(0, "listen") << std::endl;

//         } catch (const std::invalid_argument& e) {
//             std::cerr << "Test 1: " << e.what() << std::endl;
//         } catch (const std::runtime_error& e) {
//             std::cerr << "Test 1: " << e.what() << std::endl;
//         }
//     }
//     { 
//         // Test 2: non-existent file
//         ConfigParser configParser;
//         try {
//             configParser.getConfig("configs/no_file.conf");
//         } catch (const std::invalid_argument& e) {
//             std::cerr << "Test 2: " << e.what() << std::endl;
//         }
//     }
//     { 
//         // Test 3: wrong extension
//         ConfigParser configParser;
//         try {
//             configParser.getConfig("configs/lol");
//         } catch (const std::invalid_argument& e) {
//             std::cerr << "Test 3: " << e.what() << std::endl;
//         }
//     }
//     { 
//         // Test 4: file without server block
//         ConfigParser configParser;
//         try {
//             configParser.getConfig("configs/single.conf");
//         } catch (const std::invalid_argument& e) {
//             std::cerr << "Test 4: " << e.what() << std::endl;
//         }
//     }
// }


// int main()
// {
//     test();
//     return 0;

// }

 