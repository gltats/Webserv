#include <iostream>

#ifndef WEBSERVER_EXCEPTION_HPP
# define WEBSERVER_EXCEPTION_HPP
// header
class UserRequestTermination: public std::exception
{
	public:
		virtual const char* what() const throw()
		{
			return ("Exception: User Request for Termination");
		}

};

// exception implementation -> must go to cpp file (here just for testing)
// const char* UserRequestTermination::what() const throw()
// {
// 	return ("Exception: User Request for Termination");
// }
#endif