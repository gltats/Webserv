#include <iostream>

#ifndef WEBSERVER_EXCEPTION_HPP
# define WEBSERVER_EXCEPTION_HPP
// header
class UserRequestTermination: public std::exception
{
	public:
		virtual const char* what() const throw();
};

class ServerCriticalError: public std::exception
{
	public:
		virtual const char* what() const throw();
};

class InvalidRequest: public std::exception
{
	public:
		virtual const char* what() const throw();
};

class MethodNotSupported: public std::exception
{
	public:
		virtual const char* what() const throw();
};

class ResponseError: public std::exception
{
	public:
		virtual const char* what() const throw();
};

class LengthRequiredException: public std::exception
{
	public:
		virtual const char* what() const throw();
};

class EmptyResponseException: public std::exception
{
	public:
		virtual const char* what() const throw();
};

#endif
