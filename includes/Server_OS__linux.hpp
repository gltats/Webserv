/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 18:07:41 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/23 09:59:54 by mgranero         ###   ########.fr       */
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

    public:
        ServerOS(std::map<std::string, std::string> &config_map, char *env[]);
        ~ServerOS(void);
        virtual void	setup_socket(void);
        virtual void	launch(void);

};

#endif