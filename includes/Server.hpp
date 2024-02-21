
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

// file descriptors
#include <fcntl.h>

#include <stdlib.h> // for exit, is exit allowed

// socket libraries
#include <sys/socket.h>
#include <netinet/in.h> // AF_INET
#include <netdb.h>
#include <netinet/in.h> // for IPROTO_TCP protocol
#include <sys/un.h> // to suport struct_addr_un

// c basics
#include <unistd.h>
#include <stdio.h> // for tests with perror
#include <string.h>// for tests with perror
#include <errno.h> // for tests with perror
#include <map>
#include "color_code.hpp"
#include "library.hpp"

#ifndef OS_ENUM
#define OS_ENUM
	enum
	{
		MAC,
		LINUX
	};
#endif

# if defined (__linux__)
	#define OS LINUX
	#include <sys/epoll.h>
#endif

# if defined (__APPLE__)
	#define OS MAC
	#include	<sys/event.h> // for kqueue
#endif

// #ifndef SOCK_NONBLOCK
// #define SOCK_NONBLOCK 0 // remove this after splitting linux and mac implementation
// #endif




class Server
{
	private:
		std::string 						_server_port;
		std::string							_server_name;

		int									_server_socket;
		int									_connection_socket;
		int									_max_backlog_queue;

		bool								_allow_GET;
		bool								_allow_POST;
		bool								_allow_DELETE;


		struct addrinfo						*_result;

		Server(void);
		Server(Server const & src);
		Server								&operator=(Server const &rhs);
		void								_setup_server(void);
		void								_setup_socket(void);
		void								_listen(void);


	public:
		// does the server needs the map to the error pages or only the response object?
		Server(std::map<std::string, std::string> &config_map);
		~Server(void);

		int									get_server_socket(void) const;
		std::string							get_server_name(void) const;
		std::string							get_server_port(void) const;
		socklen_t							get_client_addr_size(void)const;
		struct sockaddr_un					get_client_addr(void) const;
		bool								get_allow_GET(void) const;
		bool								get_allow_POST(void) const;
		bool								get_allow_DELETE(void) const;

		void								close_server_socket(void);


};

#endif