/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 18:07:41 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/13 21:23:02 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_OS_HPP
# define SERVER_OS_HPP

# include <sys/epoll.h>

# include "Server.hpp"

class ServerOS : public Server
{
    private:
		int 			                                        _epoll_fd;
        struct epoll_event                                      _ev_server;
        int                                                    *_servers_fd; // can i remove it and replace by _server_fd_port_name?
        
        int                                                    _nb_of_servers;

        std::map<int, int>                                      _server_fd_port;
      
         bool	           _is_a_server_socket(int fd) const;

        
        void	           _print_epoll_events(uint32_t event, int fd);
        void               _close_connection(int epoll_fd, int fd_to_remove, struct epoll_event &ev_ref);
        virtual int 	   _setup_socket(int port);
        virtual void	   _loop(void);
        virtual void	    _listen_sockets(int fd_server, int port);
        virtual void	    close_server_socket(int fd);
    public:
        ServerOS(int server_index, ConfigParser &configParser, char *env[]);
        ~ServerOS(void);

        virtual void	launch_webserver(void);

};

#endif