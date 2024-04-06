/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv_Exceptions.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 19:42:56 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/06 10:46:25 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv_Exceptions.hpp"

const char* UserRequestTermination::what() const throw()
{
    return ("Exception: User Request for Termination");
}

const char* ServerCriticalError::what() const throw()
{
   return ("Exception: Server Critical Failure"); 
}

const char* InvalidRequest::what() const throw()
{
   return ("Exception: Invalid or Error in read or Request"); 
}

const char* ResponseError::what() const throw()
{
   return ("Exception: Error to generate or send Response"); 
}

const char* MethodNotSupported::what() const throw()
{
   return ("Exception: Method not supported. Webserv supports GET, POST and DELETE. \nEnable them in Config File."); 
}

const char* LengthRequiredException::what() const throw()
{
   return ("Exception: Method not supported. Webserv supports GET, POST and DELETE. \nEnable them in Config File."); 
}

const char* EmptyResponseException::what() const throw()
{
   return ("Exception: Attempt to write an empty response."); 
}
