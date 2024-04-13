/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 21:53:05 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:54:11 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <map>
# include <iostream>
# include <signal.h>
# include <stdlib.h>

# include "color_code.hpp"
# include "ConfigParser.hpp"
# include "Connection.hpp"
# include "Webserv_Exceptions.hpp"
# include "library.hpp"
# include "AServer.hpp"
# include "Server_OS__linux.hpp"

void	ft_signal_handler(int signum);
void	ft_setup_sighandler(void);

#endif