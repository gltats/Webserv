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

Connection::Connection(int connection_socket, char *env[]): _connection_socket(connection_socket), _size_data_recv(0), _flags_recv(0), _buffer_rcv_size(8192*2)
{
	std::cout << "Client connected" << std::endl;
	_env = env;
	_buffer_rcv = new char[_buffer_rcv_size];
	memset(_buffer_rcv, '\0', _buffer_rcv_size); // is memset allowed?
	// error here?
}

Connection::~Connection(void)
{

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
	memset(_buffer_rcv, '\0', _buffer_rcv_size); // is memset allowed?
	_size_data_recv = recv(_connection_socket, _buffer_rcv, _buffer_rcv_size, _flags_recv);
	if (_size_data_recv == -1)
	{
		std::cout << "Error: to recv. Reason: " << strerror(errno) << std::endl;
		std::cout << "Closing sockets and exiting" << std::endl;
		close(_connection_socket);
		// throw exception
		exit(1); // at the moment just exiting
	}
	// if (VERBOSE == 1)
	// {
	// 	std::cout << std::endl;
	// 	std::cout << "Message received is :" << YELLOW << std::endl;
	// 	std::cout << _buffer_rcv << RESET << std::endl;
	// }
	// create Request Object
	_request.read_request(_buffer_rcv);
	_request.print_request();
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

		send_size = send(_connection_socket, _response.get_response().c_str() , buffer_send_size, 0); // this works

		if (send_size != buffer_send_size)
		{
			std::cout << "Error: to send. Reason: " << strerror(errno) << std::endl;
			std::cout << "Closing sockets and exiting" << std::endl;
			close(_connection_socket);
			return ;
		}
		// std::cout << "reponse sent " << _response.get_response() << std::endl;
		std::cout << "\tnumber of characters sent " << send_size << std::endl;
		// close(_connection_socket);
	}
	else
		close(_connection_socket);
}
