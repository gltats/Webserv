/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 21:35:15 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/07 21:55:02 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

// Connection::Connection(void): _buffer_rcv_size(8192*2), _error_page_map(error_page_map)
// {

// }

Connection::Connection(int connection_socket, char *env[]): _connection_socket(connection_socket), _size_data_recv(0), _flags_recv(0), _buffer_rcv_size(8192*2), _is_read_complete(0)
{
	_env = env;
	_buffer_rcv = new char[_buffer_rcv_size];
	clear_memory(_buffer_rcv, _buffer_rcv_size);

	std::cout << "Connection created" << std::endl;

}

Connection::~Connection(void)
{

	std::cout << "Connection destroy" << std::endl;

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
			std::cout << "closing _connection_socket in Connection" << std::endl;
			delete[] _buffer_rcv;
		}
	}
}

void	Connection::receive_msg(void)
{
	clear_memory(_buffer_rcv, _buffer_rcv_size);

	_size_data_recv = recv(_connection_socket, _buffer_rcv, _buffer_rcv_size, _flags_recv);
	if (_size_data_recv == -1)
	{
		std::cout << "Error: to recv. Reason: " << strerror(errno) << " # fd = " << _connection_socket << std::endl;
		std::cout << "Closing connection sockets " << _connection_socket << " and returning" << std::endl;
		close(_connection_socket);
		
		// throw exception
		return; // at the moment just return
	}
	std::cout << "received " << _size_data_recv << " bytes" << std::endl;
	// create Request Object
	_request.read_request(_buffer_rcv);
	_request.print_request();
	_is_read_complete = 1;
}

std::string	Connection::get_response(void)
{
	return (_response.get_response());
}

void		Connection::send_response(void)
{

	size_t	send_size = 0;

	_response.create_response(_request, _env);

	if (_response.get_response().length() > 0)
	{
		size_t buffer_send_size = _response.get_response().length() + 1;

		std::cout << REDB << "trying to send to socket " << _connection_socket << ", message size is "<< buffer_send_size << RESET << std::endl;
		/* only for debugging - remove for evaluation */
		
		send_size = send(_connection_socket, _response.get_response().c_str() , buffer_send_size, 0); // this works

		if (send_size != buffer_send_size)
		{
			std::cout << "Error: to send. Reason: " << strerror(errno) << std::endl;
			std::cout << "Closing connection sockets " << _connection_socket << " and returning" << std::endl;

			close(_connection_socket);
			return ;
		}
		std::cout << "\tnumber of characters sent " << send_size << std::endl;
	}
	else
		close(_connection_socket);
}

std::string		Connection::get_connection(void) const
{
	return(_request.get_connection());
}

bool			Connection::is_response_empty(void) const
{
	if (_response.get_response().length() == 0)
		return(true);
	else 
		return(false);
}

bool			Connection::get_is_read_complete(void) const
{
	return(_is_read_complete);
}

void			Connection::set_is_read_complete(bool status)
{
	_is_read_complete = status;
}



// std::string	Connection::get_client_ip(void) const
// {

// 	// struct sockaddr_in 	*ptr4;
// 	// struct sockaddr_in6	*ptr6;
// 	// uint16_t 			port;

// 	// // Client connected throw IPv4
// 	// if(_client_addr.ssfamily == AF_INET)
// 	// {
// 	// 	ptr4 = (struct sockaddr_in *)&_client_addr;
// 	// 	port = ntohs(ptr4->sin_port);
// 	// }
// 	// // Client connected throw IPv6
// 	// else if(_client_addr.ssfamily == AF_INET6)
// 	// {
// 	// 	ptr6 = (struct sockaddr_in6 *)&_client_addr;
// 	// 	port = ntohs(ptr6->sin_port);
// 	// }
// 	// else
// 	// {
// 	// 	std::cout << "Socket family not Supported. Please use only IPv4 or IPv6 clients" << std::endl;
// 	// 	return ;
// 	// }

// }