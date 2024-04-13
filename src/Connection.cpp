/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 21:35:15 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 11:29:41 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

// Connection::Connection(void): _buffer_rcv_size(8192*2), _error_page_map(error_page_map)
// {

// }

//initialize static member of Connection
int Connection::_nb_connections = 0;

Connection::Connection(ConfigParser &configParser, int connection_socket, struct sockaddr_in &client_addr, int server_socket, int *servers_fd, char *env[]):  _configParser(configParser),  _env(env),  _connection_socket(connection_socket), _server_socket(server_socket), _servers_fd(servers_fd), _size_data_recv(0), _flags_recv(0), _buffer_rcv_size(8192*200),_request(*this), _response(configParser, _request, env), _is_read_complete(0), c_start(std::clock()), _keep_alive(true)
{
	_nb_connections++;

	// deactive keep alive if KEEP_ALIVE_TIME_SECONDS = -1
	if (KEEP_ALIVE_TIME_SECONDS == -1)
		_keep_alive = false;

	_buffer_rcv = new char[_buffer_rcv_size];

	clear_memory(_buffer_rcv, _buffer_rcv_size);

	_obtain_client_ip(client_addr);
	_obtain_client_port(client_addr);

	if (VERBOSE == 1)
		std::cout << std::endl << "Connection constructor connection " << _connection_socket << std::endl;

}

Connection::~Connection(void)
{
	_nb_connections--;
	if (VERBOSE == 1)
		std::cout << "Connection deconstructor fd "<< _connection_socket << std::endl;

	if (_buffer_rcv != 0)
	{
		if (VERBOSE == 1)
		{
			std::cout << "Free memory _buffer_rcv in Connection" << std::endl;
			delete[] _buffer_rcv;
			_buffer_rcv = 0;
		}
	}

	if (_connection_socket != -1)
	{
		if (VERBOSE == 1)
		{
			std::cout << "Closing _connection_socket fd in Connection" << std::endl << std::endl << std::endl;
			delete[] _buffer_rcv;
		}
	}
}

void				Connection::print_request(void)
{
	_request.print_request();
}

void				Connection::create_response(void)
{
	_response.create_response(_request.get_server_id());


	// std::cout << CYAN << "-------- Response Line Start --------" << std::endl;// TODO remove, only for testing
	// std::cout << "<" << _response.get_response() << ">" << std::endl; // TODO remove, only for testing
	// std::cout << "-------- Response Line Start --------" << RESET << std::endl  << std::endl; // TODO remove, only for testing
}


int					Connection::get_fd_pipe_0(void) const
{
		return ( _response.get_fd_pipe_0());
}

int					Connection::get_nb_connections(void)
{
	return (_nb_connections);
}


// void				Connection::receive_request(void)
// {
// 	clear_memory(_buffer_rcv, _buffer_rcv_size);

// 	bool is_cgi_read = false;
// 	int fd_read = -1;

// 	//if fd from a cgi read
// 	{
// 		// reading returned valus from cgi pipe.
// 		dup2(_response.get_fd_pipe_0(), STDIN_FILENO);
// 		fd_read = STDIN_FILENO;
// 		is_cgi_read = true;
// 	}
// 	else
// 	{
// 		// read from socket
// 		fd_read = _connection_socket;
// 	}

// 	_size_data_recv = recv(fd_read, _buffer_rcv, _buffer_rcv_size, _flags_recv);
// 	if (_size_data_recv == -1 || _size_data_recv == 0)
// 	{
// 		std::cout << REDB << "Error to recv : size of received is " << _size_data_recv << RESET << std::endl;
// 		std::cout << "Closing connection sockets " << _connection_socket << " and returning" << std::endl;
// 		close(_connection_socket);

// 		// throw exception
// 		return; // at the moment just return
// 	}

// 	if (is_cgi_read == true) // read from cgi socket
// 	{
// 		// return STDFILE IN to original after reading from pipe
// 		_process_response(_buffer_rcv);
// 	}
// 	else
// 	{
// 		// process read from socket
// 		_request.parse_request(_buffer_rcv);

// 		if (VERBOSE == 1)
// 			_request.print_request();
// 		_is_read_complete = 1;
// 	}







// 	// else
// 	// 	print_error_fd("No request received from connection fd ", _connection_socket);
// }

std::string				Connection::get_response(void)
{
	return (_response.get_response());
}

void		Connection::send_response(void)
{

	int	send_size = 0;

	// _response.create_response(_request.get_server_id(), _env);

	if (_response.get_response().length() > 0)
	{
		int buffer_send_size = _response.get_response().length() + 1;
		if (VERBOSE == 1)
			std::cout <<"\tTrying to send to socket " << _connection_socket << ", message size is "<< buffer_send_size << RESET << std::endl;

		// std::cout << CYAN << "response is <" << _response.get_response() << ">" << std::endl; // TODO remove

		send_size = send(_connection_socket, _response.get_response().c_str() , buffer_send_size, 0); // this works


		if (send_size == 0 ||  send_size == -1 || send_size != buffer_send_size)
		{
			std::cout << "Error: to send. Reason: " << strerror(errno) << std::endl;
			std::cout << "Closing connection sockets " << _connection_socket << " and returning" << std::endl;

			close(_connection_socket);
			return ;
		}
		if (VERBOSE == 1)
			std::cout << "\tNumber of characters actually sent " << send_size << std::endl;
		// _is_write_complete = 1;
		// _is_read_complete = 0;
		_is_read_complete = 1;
	}
	else
	{
		std::cerr << REDB << "Error: trying to write an empty _response to file descriptor " <<  _connection_socket << RESET << std::endl;
		throw EmptyResponseException();
	}
}

void					Connection::process_cgi(char const *buffer, size_t buffer_size)
{
	_response.process_cgi(buffer, buffer_size);
}

void					Connection::parse_request(char const *buffer, size_t buffer_size)
{
	const std::clock_t	c_current = std::clock();

	if (KEEP_ALIVE_TIME_SECONDS != -1 && ((c_current - c_start) / CLOCKS_PER_SEC) > KEEP_ALIVE_TIME_SECONDS)
		_keep_alive = false;
	std::cout << "Connection on socket " << _connection_socket << ", elapsed time is " << std::fixed << std::setprecision(10) <<  ((c_current - c_start) / CLOCKS_PER_SEC) << " seconds" << std::endl; // TODO remove

	// check if connection must be terminated after this request
	if (buffer_size == 0) // not used, consume
		std::cout << "";
	_request.parse_request(buffer);
}

bool					Connection::response_is_cgi(void)
{
	return(_response.get_is_cgi());
}

std::string				Connection::get_connection(void) const
{
	return(_request.get_connection());
}


bool					Connection::is_response_empty(void) const
{
	if (_response.get_response().length() == 0)
		return(true);
	else
		return(false);
}

bool					Connection::get_is_read_complete(void) const
{
	return(_is_read_complete);
}

void					Connection::set_is_read_complete(bool status)
{
	_is_read_complete = status;
}

std::string				Connection::get_client_ip(void) const
{
	return (_client_ip);
}

std::string				Connection::get_client_port(void) const
{
	return (_client_port);
}

void					Connection::_obtain_client_ip(struct sockaddr_in &client_addr)
{
	std::stringstream	ip_str;

	u_int32_t ip_addr = ntohl(client_addr.sin_addr.s_addr);

	ip_str << ((ip_addr >> 24) & 0xFF);
	ip_str << ".";
	ip_str << ((ip_addr >> 16) & 0xFF);
	ip_str << ".";
	ip_str << ((ip_addr >> 8) & 0xFF);
	ip_str << ".";
	ip_str << (ip_addr & 0xFF);

	_client_ip =  ip_str.str();
}

void					Connection::_obtain_client_port(struct sockaddr_in &client_addr)
{
	u_int16_t 			port;
	int					int_port;

	port = ntohs(client_addr.sin_port);
	int_port = port;
	_client_port = int2str(int_port);
}

int						Connection::get_error(void) const
{
	return (_response.get_ret());
	// return (_request.get_error()); // TODO get_error from Response instead of from request

}

int						*Connection::get_servers_fd(void) const
{
	return (_servers_fd);
}

ConfigParser			&Connection::get_configParser(void) const
{
	return (_configParser);
}

int						Connection::get_connection_socket(void) const
{
	return(_connection_socket);
}

int						Connection::get_server_socket(void) const
{
	return (_server_socket);
}

bool 					Connection::get_keep_alive(void) const
{
	return (_keep_alive);
}
