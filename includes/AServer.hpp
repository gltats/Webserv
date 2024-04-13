/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 18:39:04 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:48:14 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <netinet/in.h>
# include <sys/un.h>
# include <unistd.h>
# include <map>

# include "webserver_configuration.hpp"
# include "color_code.hpp"
# include "library.hpp"
# include "Connection.hpp"

class Server
{
	protected:

		// Attributes
		int									_server_index;
		ConfigParser 						&_configParser;
		int									_max_backlog_queue;
		char								**_env;
		std::map<int, Connection *>			_fd2client_map;

		// Member Functions
		virtual int							_setup_socket(int port) = 0;
		virtual void	    				_listen_sockets(int fd_server, int port) = 0;
		virtual void	   					_close_server_socket(int fd) = 0;

	public:

		// Member Functions
		Server(int server_index, ConfigParser &configParser, char *env[]);
		virtual 							~Server(void);
		virtual void						launch_webserver_loop(void)  = 0;
};

#endif