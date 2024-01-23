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

Server::Server(std::map<std::string, std::string> &config_map, std::map<std::string, std::string> &error_page_map):  _server_port(config_map["listen"]), _server_socket(-1)
{
	 _error_page_map = &error_page_map;
	try {
        // Convert the string to an integer using stoi
        _max_backlog_queue = std::stoi(config_map["limit_conn"]); // allowed function?

    } catch (const std::exception& e) {
        // Handle the exception if the conversion fails
        std::cerr << REDB <<"Conversion error: " << e.what() << RESET << std::endl;
    }

	_setup_server();
	_setup_socket();
	_listen();
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
    // struct addrinfo		*rp;

	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Stream socket for TCP */
	hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV | AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP; // only TCP allowed
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	status = getaddrinfo(NULL, _server_port.c_str(), &hints, &_result);
	if (status != 0)
	{
		std::cerr << REDB << "getaddrinfo: %s\n", gai_strerror(status); // is gai_strerror allowed?
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
		// fprintf(stderr, "listen: %s\n", strerror(errno));
		std::cerr << REDB << "Error: to listen socket. Reason: " << strerror(errno) << RESET << std::endl;
		close(_server_socket);
		//throw exception
		exit (1); // at the moment just exit
	}
}

void	Server::close_socket(void)
{
	if (_server_socket != -1)
		close(_server_socket);
	_server_socket = -1;
}