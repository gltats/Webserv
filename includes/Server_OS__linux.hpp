/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__linux.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 18:07:41 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/22 18:29:57 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_OS_HPP
# define SERVER_OS_HPP

# include <sys/epoll.h>
# include "Server.hpp"

class Server__OS : public Server
{
    private:
    public:
                Server__OS(std::map<std::string, std::string> &config_map);
                ~Server__OS(void);
                void	setup_socket(void);

};

#endif