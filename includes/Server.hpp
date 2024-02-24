
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
		std::string 						_server_port;
		std::string							_server_name;

		int									_server_socket;
		int									_connection_socket;
		int									_max_backlog_queue;

		
		struct addrinfo						*_result;
		char								**_env;

		// map to hold Connection Object pointer per file descriptor
		std::map<int, Connection *>			_fd2client_map;
		int 								_epoll_fd;

		std::map<std::string, std::string> 	&_config_map;



		// Server(void);
		// Server(Server const & src);
		Server								&operator=(Server const &rhs);
		void								_setup_server(void);


	public:
		// does the server needs the map to the error pages or only the response object?
		Server(std::map<std::string, std::string> &config_map, char *env[]);

		virtual 							~Server(void);

		virtual void						setup_socket(void) = 0;
		void								listen_socket(void);


		int									get_server_socket(void) const;
		std::string							get_server_name(void) const;
		std::string							get_server_port(void) const;

		bool								get_allow_GET(void) const;
		bool								get_allow_POST(void) const;
		bool								get_allow_DELETE(void) const;

		void								close_server_socket(void);
    	virtual void						launch(void) = 0;
		void								launch_webserver(void);

};

#endif