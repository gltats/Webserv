/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 18:07:41 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:24:52 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_OS_HPP
# define SERVER_OS_HPP

# include <sys/epoll.h>
# include "AServer.hpp"

class ServerOS : public Server
{
	private:
		// Attributes
		int 											_epoll_fd;
		struct epoll_event								_ev_server;
		int												*_servers_fd;
		int												_nb_of_servers;
		int												_flags_recv;
		std::map<int, int>								_server_fd_port;
		std::map<int, int>								_cgi_fd_2_connection_fd;

		// Member Functions
		bool											_is_a_server_socket(int fd) const;
		bool											_is_key_in_map(std::map<int, Connection *> *map, int key);
		void											_print_epoll_events(uint32_t event, int fd);
		void											_close_connection(int epoll_fd, int fd_to_remove, struct epoll_event &ev_ref);
		int												_is_port_already_set(int port, int *array_ports_set, int size_array);
		virtual int 									_setup_socket(int port);
		virtual void									_listen_sockets(int fd_server, int port);
		virtual void									_close_server_socket(int fd);

	public:
		// Member Functions
		ServerOS(int server_index, ConfigParser &configParser, char *env[]);
		~ServerOS(void);
		virtual void									launch_webserver_loop(void);
};

#endif