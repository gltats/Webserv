/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 18:07:41 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/29 21:15:14 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_OS_HPP
# define SERVER_OS_HPP

# include <sys/epoll.h>

# include "Server.hpp"

class ServerOS : public Server
{
    private:
        void	        _print_epoll_events(uint32_t event, int fd);
        void            _close_connection(int epoll_fd, int fd_to_remove, struct epoll_event &ev_ref);
        virtual void	_setup_socket(void);
        virtual void	_loop(void);
    
    public:
        ServerOS(int server_index, ConfigParser &configParser, char *env[]);
        ~ServerOS(void);

        virtual void	launch_webserver(void);

};

#endif