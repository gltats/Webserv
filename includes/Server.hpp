
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

// file descriptors
#include <fcntl.h>

#include <stdlib.h> // for exit, is exit allowed
#include <sstream> // for converter of string to int -> add to a support function lib

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


class Server
{
	private:
		std::string 						_server_port;
		int									_server_socket;
		int									_connection_socket;
		int									_max_backlog_queue;
		// socklen_t						_client_addr_size;
		// struct sockaddr_un				_client_addr;
		std::map<std::string, std::string> *_error_page_map;

		struct addrinfo			*_result;

		Server(void);
		Server(Server const & src);
		Server					&operator=(Server const &rhs);
		void					_setup_server(void);
		void					_setup_socket(void);
		void					_listen(void);


	public:
		// does the server needs the map to the error pages or only the response object?
		Server(std::map<std::string, std::string> &config_map, std::map<std::string, std::string> &error_page_map);
		~Server(void);

		int						get_server_socket(void) const;
		std::string				get_server_port(void) const;
		socklen_t				get_client_addr_size(void)const;
		struct sockaddr_un		get_client_addr(void) const;

		void					close_socket(void);


};

#endif