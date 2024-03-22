/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 21:35:15 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/21 21:30:39 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

// Connection::Connection(void): _buffer_rcv_size(8192*2), _error_page_map(error_page_map)
// {

// }

Connection::Connection(ConfigParser &configParser, int connection_socket, struct sockaddr_in &client_addr, int server_socket, int *servers_fd, char *env[]):  _configParser(configParser),  _env(env),  _connection_socket(connection_socket), _server_socket(server_socket), _servers_fd(servers_fd), _size_data_recv(0), _flags_recv(0), _buffer_rcv_size(8192*200),_request(*this), _response(configParser, _request, env), _is_read_complete(0)
{

	_buffer_rcv = new char[_buffer_rcv_size];

	clear_memory(_buffer_rcv, _buffer_rcv_size);

	_obtain_client_ip(client_addr);
	_obtain_client_port(client_addr);

	if (VERBOSE == 1)
		std::cout << "Connection constructor" << std::endl;

}

Connection::~Connection(void)
{
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
			std::cout << "Closing _connection_socket fd in Connection" << std::endl;
			delete[] _buffer_rcv;
		}
	}
}

void				Connection::receive_request(void)
{
	clear_memory(_buffer_rcv, _buffer_rcv_size);

	_size_data_recv = recv(_connection_socket, _buffer_rcv, _buffer_rcv_size, _flags_recv);
	if (_size_data_recv == -1)
	{
		// std::cout << "Error: to recv. Reason: " << strerror(errno) << " # fd = " << _connection_socket << std::endl;
		std::cout << "Closing connection sockets " << _connection_socket << " and returning" << std::endl;
		close(_connection_socket);

		// throw exception
		return; // at the moment just return
	}
	if (_size_data_recv > 0)
	{
		_request.parse_request(_buffer_rcv);

		if (VERBOSE == 1)
			_request.print_request();
		_is_read_complete = 1;
	}
	// else
	// 	print_error_fd("No request received from connection fd ", _connection_socket);
}

std::string				Connection::get_response(void)
{
	return (_response.get_response());
}

void		Connection::send_response(void)
{

	size_t	send_size = 0;

	_response.create_response(_request.get_server_id(), _env);

	if (_response.get_response().length() > 0)
	{
		size_t buffer_send_size = _response.get_response().length() + 1;
		if (VERBOSE == 1)
			std::cout <<"\tTrying to send to socket " << _connection_socket << ", message size is "<< buffer_send_size << RESET << std::endl;

		send_size = send(_connection_socket, _response.get_response().c_str() , buffer_send_size, 0); // this works

		if (send_size != buffer_send_size)
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
		close(_connection_socket);
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
	return (_request.get_error());

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
