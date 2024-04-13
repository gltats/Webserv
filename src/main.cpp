/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 17:06:43 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:24:36 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

/*
	run it by calling ./webserver configs/default.conf
*/

/*
	Comments:
		-exit() is not allowed
		-AServer is not in canonical form


*/

// TODO header for main.cpp which represents all of us

// TODO debug filetransfer
/*
File Transfer name not found
Exception: Status 400, Bad Request

*/

// TODO if errror in sendv, recv, read, write client is removed

// TODO if errror in sendv, recv, read, write check return value for 0 and -1
// TODO check for errno after a read or write
// TODO read body size from server block and use it in the server

// TODO keepalive_timeout and keepalive_requests
/*
The default values for keep-alive connections in Nginx are as follows:

keepalive_timeout: By default, Nginx sets this to 75 seconds.
keepalive_requests: By default, Nginx sets this to 100 requests per keep-alive connection.
*/
// TODO Chunk extensions and passing key,values within
// TODO File Transfer with multipart/form-data

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
// INFO risk: if python does not find the file it hangs (with Maira MVP for response)

// TODO check the requirement in subject for : all read and write inside the loop
// TODO check other mandatory checks for request
// TODO what should happen if there is more than one server with default_server flag for the same port in the config file
// TODO testing with telnet ok_POST_body_1char -> NO Body and content lenght doubled
/*
    Key:<content-lenght> | Value:<1>
        Key:<content-length> | Value:<0>
	*/
// TODO issue to get user temrination after error
/*
	Error:
Connection closed. Please retry
^CGeneric Exception during receive_request
Exception: User Request for Termination
^C^C^C^C==519==
==519== Process terminating with default action of signal 15 (SIGTERM)
==519==    at 0x4BD666E: recv (recv.c:28)
==519==    by 0x126033: Connection::receive_request() (in /42PROJECT/webserver)
==519==    by 0x118077: ServerOS::_loop() (in /42PROJECT/webserver)
==519==    by 0x116A32: ServerOS::launch_webserver() (in /42PROJECT/webserver)
==519==    by 0x10CE29: main (in /42PROJECT/webserver)
==519==
==519== FILE DESCRIPTORS: 9 open (3 std) at exit.
==519== Open AF_INET socket 8: 127.0.0.1:4432 <-> 127.0.0.1:44474
==519==    at 0x4BD6427: accept (accept.c:26)
==519==    by 0x117AD6: ServerOS::_loop() (in /42PROJECT/webserver)
==519==    by 0x116A32: ServerOS::launch_webserver() (in /42PROJECT/webserver)
==519==    by 0x10CE29: main (in /42PROJECT/webserver)
==519==
==519== Open AF_INET socket 7: 127.0.0.1:4432 <-> 127.0.0.1:39768
==519==    at 0x4BD6427: accept (accept.c:26)
==519==    by 0x117AD6: ServerOS::_loop() (in /42PROJECT/webserver)
==519==    by 0x116A32: ServerOS::launch_webserver() (in /42PROJECT/webserver)
==519==    by 0x10CE29: main (in /42PROJECT/webserver)
==519==
==519== Open AF_INET socket 6: 0.0.0.0:4431 <-> unbound
==519==    at 0x4BD6B3B: socket (syscall-template.S:120)
==519==    by 0x116A6C: ServerOS::_setup_socket(int) (in /42PROJECT/webserver)
==519==    by 0x11661F: ServerOS::ServerOS(int, ConfigParser&, char**) (in /42PROJECT/webserver)
==519==    by 0x10CE1A: main (in /42PROJECT/webserver)
==519==
==519== Open AF_INET socket 5: 0.0.0.0:4433 <-> unbound
==519==    at 0x4BD6B3B: socket (syscall-template.S:120)
==519==    by 0x116A6C: ServerOS::_setup_socket(int) (in /42PROJECT/webserver)
==519==    by 0x11661F: ServerOS::ServerOS(int, ConfigParser&, char**) (in /42PROJECT/webserver)
==519==    by 0x10CE1A: main (in /42PROJECT/webserver)
==519==
==519== Open AF_INET socket 4: 0.0.0.0:4432 <-> unbound
==519==    at 0x4BD6B3B: socket (syscall-template.S:120)
==519==    by 0x116A6C: ServerOS::_setup_socket(int) (in /42PROJECT/webserver)
==519==    by 0x11661F: ServerOS::ServerOS(int, ConfigParser&, char**) (in /42PROJECT/webserver)
==519==    by 0x10CE1A: main (in /42PROJECT/webserver)
==519==
==519== Open file descriptor 3:
==519==    at 0x4BD5B1B: epoll_create (syscall-template.S:120)
==519==    by 0x116373: ServerOS::ServerOS(int, ConfigParser&, char**) (in /42PROJECT/webserver)
==519==    by 0x10CE1A: main (in /42PROJECT/webserver)
==519==
==519==
==519== HEAP SUMMARY:
==519==     in use at exit: 121,839 bytes in 190 blocks
==519==   total heap usage: 442 allocs, 252 frees, 168,715 bytes allocated
==519==
==519== LEAK SUMMARY:
==519==    definitely lost: 0 bytes in 0 blocks
==519==    indirectly lost: 0 bytes in 0 blocks
==519==      possibly lost: 0 bytes in 0 blocks
==519==    still reachable: 121,839 bytes in 190 blocks
==519==         suppressed: 0 bytes in 0 blocks
==519== Reachable blocks (those to which a pointer was found) are not shown.
==519== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==519==
==519== For lists of detected and suppressed errors, rerun with: -s
==519== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
Terminated
root@91711e67e4c7:/42PROJECT#

*/
// TODO Error in DELETE  nok_DELETE_with_body - its is saying
/*
	Content-Length does not match the size of the body received. Content-Length is 10,body is 14
	Exception: Status 400, Bad Request
*/

// TODO remove content lenght check with body size. as in body there is also the CRLF and traler, chunk ...

// DONE file transfer request request example : "multipart/form-data"-

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

	ft_setup_sighandler();

	try
	{
		// constructor setup socket and put it in listening mode
		ServerOS srv(0, configParser, env);
		srv.launch_webserver_loop();
	}
	catch(const UserRequestTermination& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(const ServerCriticalError& e)
	{
		std::cout << e.what() << std::endl;
		return (1);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return (0);
}