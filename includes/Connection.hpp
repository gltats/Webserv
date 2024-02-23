/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 21:30:09 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/23 11:18:32 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> // for exit
#include <cerrno> // for errno : only pre-development
// #include <netinet/in.h> // for sockaddr_un support
// #include <sys/socket.h> // for sockaddr_un support
#include <sys/un.h> // for sockaddr_un support
#include <arpa/inet.h> // for htons...

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
		bool					_is_read_complete;
		std::string				_client_ip;
		std::string				_client_port;

		void					_obtain_client_ip(struct sockaddr_in &client_addr);
		void					_obtain_client_port(struct sockaddr_in &client_addr);

		// Connection(Connection const &src);
		// Connection		&operator=(Connection const &rhs);
		// Connection(void);
	public:
		Connection(int connection_socket, struct sockaddr_in &_client_addr, char *env[]);
		~Connection(void);

		void					receive_request(void);
		std::string				get_response(void);
		void					send_response(void);
		std::string				get_connection(void) const;
		bool					is_response_empty(void) const;
		bool 					get_is_read_complete(void) const;
		void					set_is_read_complete(bool status); // is it required?
		std::string				get_client_ip(void) const;
		std::string				get_client_port(void) const;
};

#endif