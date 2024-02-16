/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 21:30:09 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/04 21:18:42 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> // for exit
#include <cerrno> // for errno : only pre-development
#include <string.h> // for memset: check if it is allowed
// #include <netinet/in.h> // for sockaddr_un support
// #include <sys/socket.h> // for sockaddr_un support
#include <sys/un.h> // for sockaddr_un support

#include "color_code.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Connection
{
	private:
		int						_connection_socket;
		ssize_t					_size_data_recv;
		int						_flags_recv;
		const size_t			_buffer_rcv_size;
		char					*_buffer_rcv;
		char					**_env;
		Request 				_request;
		Response				_response;

		// Connection(Connection const &src);
		// Connection		&operator=(Connection const &rhs);
		// Connection(void);
	public:
		Connection(int connection_socket, char *env[]);
		~Connection(void);

		void					receive_msg(void);
		std::string				get_response(void);
		void					send_response(void);
		std::string				get_connection(void) const;
		// std::string				get_client_ip(void) const;

};

#endif