/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   library.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 20:15:06 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/04 20:27:47 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRARY_HPP
# define LIBRARY_HPP

# include <iostream>
# include <sstream>
# include <stdlib.h>
# include "Webserv_Exceptions.hpp"
# include <color_code.hpp>

std::string	int2str(int nb);
int     	str2int(std::string);
void	    clear_memory(void *dest, size_t size);
void	    print_error(std::string error_msg);
void	    print_error_fd(std::string error_msg, int fd);
void	    print_error_error_exit(std::string error_msg, int exit_code);
// void	    print_error_throw_exception(std::string &error_msg, std::exception except_class);

#endif