/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 21:30:09 by mgranero          #+#    #+#             */
/*   Updated: 2024/01/23 21:07:21 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#include "color_code.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Connection
{
	private:
		std::map<std::string, std::string> &_error_page_map;
		int						_connection_socket;
		ssize_t					_size_data_recv;
		int						_flags_recv;
		const size_t			_buffer_rcv_size;
		char					*_buffer_rcv;
		// socklen_t				_client_addr_size;
		// struct sockaddr_un		_client_addr;
		char					**_env;
		Request 				_request;
		Response				_response;


		// Connection(Connection const &src);
		// Connection		&operator=(Connection const &rhs);
		// Connection(void);
	public:
		Connection(int connection_socket, char *env[], std::map<std::string, std::string> &error_page_map);
		~Connection(void);

		void					receive_msg(void);
		std::string				get_response(void);
		void					send_response(void);

};

#endif