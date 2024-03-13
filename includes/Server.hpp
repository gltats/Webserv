
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
#include <map>

// project basics
#include "color_code.hpp"
#include "library.hpp"
#include "Connection.hpp"

#ifndef VERBOSE
#define VERBOSE 0
#endif

class Server
{
	protected:
		int									_server_index;
		ConfigParser 						&_configParser;


		// std::string 						_server_port;
		// std::string							_server_name;

		// int									_server_socket;
		// int									_connection_socket;
		int									_max_backlog_queue;

		char								**_env;

		// map to hold Connection Object pointer per file descriptor
		std::map<int, Connection *>			_fd2client_map;


		// Server(void);
		// Server(Server const & src);
		Server								&operator=(Server const &rhs);
		// void								_setup_server(void);
		virtual int							_setup_socket(int port) = 0;
		virtual void						_loop(void) = 0;
		virtual void	    				_listen_sockets(int fd_server, int port) = 0;


	public:
		// does the server needs the map to the error pages or only the response object?
		Server(int server_index, ConfigParser &configParser, char *env[]);

		virtual 							~Server(void);

		virtual void						launch_webserver(void)  = 0;


		// int									get_server_socket(void) const;
		// std::string							get_server_name(void) const;
		// std::string							get_server_port(void) const;

		// bool								get_allow_GET(void) const;
		// bool								get_allow_POST(void) const;
		// bool								get_allow_DELETE(void) const;
		virtual void	   					 close_server_socket(int fd) = 0;

};

#endif