/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   library.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 20:15:45 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/04 20:28:04 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "library.hpp"

std::string	size_t2str(size_t nb)
{
	std::stringstream	nb2str;
	std::string			str;

	nb2str << nb;
	str = nb2str.str();
	return (str);
}


std::string	int2str(int nb)
{
	std::stringstream	nb2str;
	std::string			str;

	nb2str << nb;
	str = nb2str.str();
	return (str);
}

int			str2int(std::string str)
{
	std::stringstream	str2nb;
	int					nb;

	str2nb << str;
	str2nb >> nb;

	if (str2nb.fail() == true)
	{
		std::cerr << REDB << "Error:\nConversion failed in str2int. The input string '";
		std::cerr << str << "' is not a valid integer. Returned -1" << RESET << std::endl;
		nb = -1;
	}
	return (nb);
}

void		clear_memory(void *dest, size_t size)
{
	unsigned char	*u_str;

	if (dest == NULL || size <= 0)
	{
		std::cerr << REDB << "Error:\nclear_memory arguments" << RESET << std::endl;
		return ;
	}
	u_str = (unsigned char *) dest;

	for (size_t i = 0; i < size; i++)
	{
		u_str[i] = (unsigned char)'\0';
	}
}

void		print_error(std::string error_msg)
{
	std::cerr << REDB << "Error:\n" << error_msg << RESET << std::endl;
}

void	    print_error_fd(std::string error_msg, int fd)
{
	std::cerr << REDB << "Error:\n";
	std::cerr << error_msg << " " << fd << RESET << std::endl;
}
