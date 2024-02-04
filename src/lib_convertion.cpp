/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lib_convertion.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 20:15:45 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/04 20:28:04 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib_convertion.hpp"

std::string	int2str(int nb)
{
	std::stringstream	nb2str;
	std::string			str;

	nb2str << nb;
	str = nb2str.str();
	return (str);
}

int	str2int(std::string str)
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