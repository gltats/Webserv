#include "Response.hpp"

void Response::getMethod()
{
	if (_ret <= 301 || _ret >= 307)
	{
		// if (_ret != 200 || !readObject())
		if (!readObject() || _ret != 200) // inverted as readObject will return 200 if sucess
			std::cout << REDB << "Error in function : getMethod reason: ret = _" << _ret << RESET << std::endl; // TODO remove
			
			// std::cout << "Error\n";
	}
}


/* ************************************************************************** */

int Response::readObject()
{
	if (_cgi)
		return (runcgi());
	else if (isFile())
		return(readFile());
	else if (isIndex())
		return(readDefault());
	return (_ret = 404, 0);
	if (isFile())
		return(readFile());
	return(_ret = 404, 0);
 }