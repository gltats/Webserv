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
		std::cerr << REDB << "Error:\nConversion failed in str2int. The input string '" << str << "' is not a valid integer." << RESET << std::endl;
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

	for (int i = 0; i < size; i++)
	{
		u_str[i] = (unsigned char)'\0';
	}
}

void		print_error(std::string &error_msg)
{
	std::cerr << REDB << "Error:\n" << error_msg << RESET << std::endl;
}

void	    print_error_error_exit(std::string &error_msg, int exit_code)
{
	std::cerr << REDB << "Error:\n" << error_msg << RESET << std::endl;
	exit(exit_code);
}

// this function must be tested
// template<typename ExceptionType>
// void	    print_error_throw_exception(std::string &error_msg, std::exception except_class)
// {
// 	std::cerr << REDB << "Error:\n" << error_msg << RESET << std::endl;
// 	throw ExceptionType();
// }
