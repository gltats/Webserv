#include "Server.hpp"

// Server::Server(void)
// {

// }
// Server::Server(Server const &src)
// {
// 	(void)src;
// }

Server					&Server::operator=(Server const &rhs)
{
	(void) rhs;
	return (*this);
}

// server_port, and config_map must come from a getter from configParser
// Server::Server(ConfigParser &configParser, char *env[]):  _configParser(configParser), _server_port(_configParser.get_listen()), _server_name(_configParser.get_server_name()), _server_socket(-1), _env(env)
Server::Server(int server_index, ConfigParser &configParser, char *env[]): _server_index(server_index), _configParser(configParser), _server_port("4432"), _server_name("42webserver"), _server_socket(-1), _env(env)
{
	
	// _max_backlog_queue = _configParser.get_limit_conn();
	_max_backlog_queue = 128; // SOMAXXCON


	std::cout << "Server constructor" << std::endl;

	_setup_server();
	std::cout << "Server Setup done" << std::endl;


}

Server::~Server(void)
{
	if (_server_socket != -1)
	{
		if (VERBOSE == 1)
				std::cout << "Closing server socket " << std::endl;
		close(_server_socket);
		_server_socket = -1;
	}
}

int			Server::get_server_socket(void) const
{
	return (_server_socket);
}

std::string	Server::get_server_port(void) const
{
	return (_server_port);
}

void Server::_setup_server(void)
{
	// if (OS == MAC)
	// 	std::cout << "Setup Webserver for Mac OS" << std::endl;
	// else if (OS == LINUX)
	// 	std::cout << "Setup Webserver for Linux" << std::endl;
	// else
	// {
	// 	std::cout << "Setup Webserver not supported for operational system. Exiting application" << std::endl;
	// 	exit(1);
	// }
}

// void	Server::_setup_socket(void)
// {

// 	_server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// 	if (_server_socket == -1)
// 	{
// 		std::cerr << REDB << "Error: to create Server Socket" << RESET << std::endl;
// 		//throw exception
// 		exit (1); // at the moment just exit
// 	}
// 	std::cout << "Socket created sucessfully" << std::endl;

// 	int yes = 1;
// 	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
// 	{
// 		close(_server_socket);
// 		std::cerr << REDB << "Error setsockopt: " << strerror(errno) << RESET << std::endl; // is allowed?
// 		//throw exception
// 		exit (1); // at the moment just exit
// 	}
// 	std::cout << "Socket option SO_REUSEADDR set sucessfully" << std::endl;
	
// 	// if (OS == MAC)
// 	// {
// 		int flags = fcntl(_server_socket, F_GETFL, 0, 0); // remove - illegal function
// 		// std::cout << "flags standard on _server_socket " << flags << std::endl; // remove - illegal function
// 		// flags = 2;
// 		int ret = 0;
// 		//  ret =  fcntl(_server_socket, F_SETFL, flags | O_NONBLOCK);;
// 		//  ret =  fcntl(_server_socket, F_SETFL, flags | O_NONBLOCK, FD_CLOEXEC);
// 		if (ret == -1)
// 		{
// 			std::cout << REDB <<  "Error: to set socket to O_NONBLOCK for MAC applications. Reason: " << strerror(errno) << RESET << std::endl;
// 			close(_server_socket);
// 			//throw exception
// 			exit (1); // at the moment just exit
// 		}
// 	// }	

// 	struct sockaddr_in serverAddr;
// 	clear_memory(&serverAddr, sizeof(serverAddr));

// 	serverAddr.sin_family = AF_INET;
// 	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all available interfaces
// 	serverAddr.sin_port = htons(str2int(_server_port)); // PORT is the port number
// 	if (bind(_server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
// 	// if (bind(_server_socket, _result->ai_addr, _result->ai_addrlen) == -1)
// 	{
// 		std::cout << REDB <<  "Error: to Bind socket and server_sockaddr. Reason: " << strerror(errno) << RESET << std::endl;
// 		close(_server_socket);
// 		//throw exception
// 		exit (1); // at the moment just exit
// 	}
// 	std::cout << "Binding sucessful" << std::endl;
// }

void	Server::_listen_socket(void)
{
	if (listen(_server_socket, _max_backlog_queue) == -1)
	{
		print_error("to listen socket");
		close(_server_socket);
		//throw exception
		exit (1); // at the moment just exit
	}
}


void	Server::close_server_socket(void)
{
	if (_server_socket != -1)
		close(_server_socket);
	_server_socket = -1;
}


std::string		Server::get_server_name(void) const
{
	return (_server_name);
}


