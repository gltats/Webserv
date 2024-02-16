#include "Server.hpp"

Server::Server(void)
{

}
Server::Server(Server const &src)
{
	(void)src;
}

Server					&Server::operator=(Server const &rhs)
{
	(void) rhs;
	return (*this);
}

Server::Server(std::map<std::string, std::string> &config_map):  _server_port(config_map["listen"]), _server_socket(-1), _server_name(config_map["server_name"])
{
	if (config_map["allow_GET"].compare("y") == 0)
		_allow_GET = true;
	else
		_allow_GET = false;

	if (config_map["allow_POST"].compare("y") == 0)
		_allow_POST = true;
	else
		_allow_POST = false;

	if (config_map["allow_DELETE"].compare("y") == 0)
		_allow_DELETE = true;
	else
		_allow_DELETE = false;

	_max_backlog_queue = str2int(config_map["limit_conn"]);


	std::cout << "Server constructor" << std::endl;

	_setup_server();
	std::cout << "Server Setup done" << std::endl;
	_setup_socket();
	std::cout << "Socket Setup done" << std::endl;
	_listen();
	std::cout << "Socket in listenning mdoe" << std::endl;

}

Server::~Server(void)
{
	if (_server_socket != -1)
	{
		if (VERBOSE == 1)
				std::cout << "Freeing addrinfo" << std::endl;
		close(_server_socket);
	}
	if (_result != 0)
	{
		if (VERBOSE == 1)
			std::cout << "Socket_server_fd closed" << std::endl;
		freeaddrinfo(_result);
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
	int					status;
	struct addrinfo		hints;

	if (OS == MAC)
		std::cout << "Setup Webserver for Mac OS" << std::endl;
	else if (OS == LINUX)
		std::cout << "Setup Webserver for Linux" << std::endl;
	else
	{
		std::cout << "Setup Webserver not supported for operational system. Exiting application" << std::endl;
		// throw exception
		exit(1); // at the moment
	}
	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_INET;    // Allow IPv4
	// if (OS == MAC)
		hints.ai_socktype = SOCK_STREAM; // Stream socket - only for mac
	// else
		// hints.ai_socktype = SOCK_STREAM | SOCK_NONBLOCK; // only for linux
	hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV | AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP; // only TCP allowed
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;


//    status = getaddrinfo(_server_name.c_str(), _server_port.c_str(), &hints, &_result);
   status = getaddrinfo(NULL, _server_port.c_str(), &hints, &_result);

	if (status != 0)
	{
		std::cerr << REDB << "Error: getaddrinfo nb " << status << " - " << gai_strerror(status) << std::endl; // is gai_strerror allowed?
		// throw exception
		exit(1); // at the moment just exit
	}

	// // result is a list and can be looped to show additional connections. we just want one
	if (_result->ai_next != NULL)
	{
		std::cerr << REDB << "Error: result->ai_next does not point to zero. more lists returned" << RESET << std::endl;
		// throw exception
		exit(1); // at the moment just exit
	}

}

void	Server::_setup_socket(void)
{

	_server_socket = socket(_result->ai_family, _result->ai_socktype, _result->ai_protocol);
	if (_server_socket == -1)
	{
		std::cerr << REDB << "Error: to create Server Socket" << RESET << std::endl;
		//throw exception
		exit (1); // at the moment just exit
	}
	std::cout << "Socket created sucessfully" << std::endl;

	int yes = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		close(_server_socket);
		std::cerr << REDB << "Error setsockopt: " << strerror(errno) << RESET << std::endl; // is allowed?
		//throw exception
		exit (1); // at the moment just exit
	}
	std::cout << "Socket option SO_REUSEADDR set sucessfully" << std::endl;
	
	if (OS == MAC)
	{
		int flags = fcntl(_server_socket, F_GETFL, 0, 0); // remove - illegal function
		std::cout << "flags standard on _server_socket " << flags << std::endl; // remove - illegal function
		// flags = 2;
		int ret =  fcntl(_server_socket, F_SETFL, flags | O_NONBLOCK);;
		// int ret =  fcntl(_server_socket, F_SETFL, flags | O_NONBLOCK, FD_CLOEXEC);
		if (ret == -1)
		{
			std::cout << REDB <<  "Error: to set socket to O_NONBLOCK for MAC applications. Reason: " << strerror(errno) << RESET << std::endl;
			close(_server_socket);
			//throw exception
			exit (1); // at the moment just exit
		}
	}	

	if (bind(_server_socket, _result->ai_addr, _result->ai_addrlen) == -1)
	{
		std::cout << REDB <<  "Error: to Bind socket and server_sockaddr. Reason: " << strerror(errno) << RESET << std::endl;
		close(_server_socket);
		//throw exception
		exit (1); // at the moment just exit
	}
	std::cout << "Binding sucessful" << std::endl;
}

void	Server::_listen(void)
{
	if (listen(_server_socket, _max_backlog_queue) == -1)
	{
		std::cerr << REDB << "Error: to listen socket. Reason: " << strerror(errno) << RESET << std::endl; // not allowd to use errno
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

bool	Server::get_allow_GET(void) const
{
	return (_allow_GET);
}

bool	Server::get_allow_POST(void) const
{
	return (_allow_POST);
}

bool	Server::get_allow_DELETE(void) const
{
	return (_allow_DELETE);
}

std::string		Server::get_server_name(void) const
{
	return (_server_name);
}


