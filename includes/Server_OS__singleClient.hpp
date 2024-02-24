/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_OS__singleClient.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 18:07:41 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/23 09:59:54 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Basic implementation for basic request and response development with generic
   implementation for MAC and Linux.
   Only One Client at the Time and Connection is closed after sending a 
   Response
*/

#ifndef SERVER_OS_HPP
# define SERVER_OS_HPP

# include "Server.hpp"

class ServerOS : public Server
{
    public:
        ServerOS(std::map<std::string, std::string> &config_map, char *env[]);
        ~ServerOS(void);
        virtual void	setup_socket(void);
        virtual void	launch(void);
};

#endif