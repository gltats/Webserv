/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 21:30:09 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/23 11:36:17 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include <cerrno>
#include <sys/un.h> 
#include <arpa/inet.h> 
#include "color_code.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Connection
{
	private:
		ConfigParser 						&_configParser;
		char								**_env;
		int									_connection_socket;
		int									_server_socket;
		int									*_servers_fd;
		ssize_t								_size_data_recv;
		int									_flags_recv;
		const size_t						_buffer_rcv_size;
		char								*_buffer_rcv;
		Request 							_request;
		Response							_response;
		bool								_is_read_complete;
		// bool					_			is_write_complete;

		std::string							_client_ip;
		std::string							_client_port;

		void								_obtain_client_ip(struct sockaddr_in &client_addr);
		void								_obtain_client_port(struct sockaddr_in &client_addr);

		// Connection(Connection const &src);
		// Connection		&operator=(Connection const &rhs);
		// Connection(void);
	public:
		Connection(ConfigParser &configParser, int connection_socket, struct sockaddr_in &_client_addr, int server_socket, int *servers_fd, char *env[]);
		~Connection(void);

		void								receive_request(void);
		std::string							get_response(void);
		void								send_response(void);
		std::string							get_connection(void) const;
		bool								is_response_empty(void) const;
		bool 								get_is_read_complete(void) const;
		void								set_is_read_complete(bool status); // is it required?

		std::string							get_client_ip(void) const;
		std::string							get_client_port(void) const;

		int									get_error(void) const;
		int									*get_servers_fd(void) const;
		ConfigParser						&get_configParser(void) const;
		int									get_connection_socket(void) const;
		int									get_server_socket(void) const;

};

#endif