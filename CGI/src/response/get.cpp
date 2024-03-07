#include "../../../includes/response_arafa.hpp"

void Response::getMethod()
{
	if (_ret <= 301 || _ret >= 307)
	{
		if (_ret != 200 || !readObject())
			error();
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
}