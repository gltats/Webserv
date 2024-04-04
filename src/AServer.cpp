#include "AServer.hpp"

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
Server::Server(int server_index, ConfigParser &configParser, char *env[]): _server_index(server_index), _configParser(configParser), _env(env)
{
	
	// _max_backlog_queue = _configParser.get_limit_conn();
	_max_backlog_queue = 128; // SOMAXXCON


	std::cout << "Server constructor" << std::endl;

	// _setup_server();
	std::cout << "Server Setup done" << std::endl;

	std::cout << "Data from Config" << std::endl;

	// configParser.build_map();

	std::cout << " Nb of servers " << configParser.getNumServers() << std::endl;
	for (size_t i = 0; i < configParser.getNumServers(); i++)
	{
			std::cout << std::endl;
			std::cout << " -- Server " << i << " -- " << std::endl;
			std::cout << "listen port : " << configParser.getParameterValue(i, "listen") << std::endl;
			std::cout << "server name : " << configParser.getParameterValue(i, "server_name") << std::endl;
	}


}

Server::~Server(void)
{
	// if (_server_socket != -1)
	// {
	// 	if (VERBOSE == 1)
	// 			std::cout << "Closing server socket " << std::endl;
	// 	close(_server_socket);
	// 	_server_socket = -1;
	// }
}

// int			Server::get_server_socket(void) const
// {
// 	return (_server_socket);
// }

// std::string	Server::get_server_port(void) const
// {
// 	return (_server_port);
// }

// void Server::_setup_server(void)
// {
// 	// do we need this function?
// }

// void	Server::_listen_socket(void)
// {
// 	if (listen(_server_socket, _max_backlog_queue) == -1)
// 	{
// 		print_error("to listen socket");
// 		close(_server_socket);
// 		//throw exception
// 		exit (1); // at the moment just exit
// 	}

	
// }


// void	Server::close_server_socket(int fd)
// {
// 	int nb_servers = configParser.get_nb_of_servers();
// 	for (int i = 0; i < nb_servers; i++)
// 	{

// 	}

// 	// if (_server_socket != -1)
// 	// 	close(_server_socket);
// 	// _server_socket = -1;
// }


// std::string		Server::get_server_name(void) const
// {
// 	return (_server_name);
// }


