#include "Response.hpp"

void Response::getMethod()
{
	if (_ret <= 301 || _ret >= 307)
	{
		if (_ret != 200 || !readObject())
			std::cout << "Error\n";
	}
}


/* ************************************************************************** */

int Response::readObject()
{
// 	if (_cgi)
// 		return (runcgi());
// 	else if (isFile())
// 		return(readFile());
// 	else if (isIndex())
// 		return(readDefault());
// 	return (_ret = 404, 0);
	// if (isFile())
	// 	return (
		return(readFile());
	// return(_ret = 404, 0);
 }