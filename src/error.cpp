#include "Response.hpp"

void Response::error()
{
	for (size_t i = 0; i < _server.error_page.size(); i++)
	{
		if (_ret == _server.error_page[i].status_code)
				spError(server.error_page[i].page);
	}
	defError(); //^ return the default error page 
}

/* ************************************************************************** */



void Response::spError(std::string page)
{
	_Resbody = page;
}

/* ************************************************************************** */


void Response::defError()
{
	_Resbody = setErrorPage("./frontEnd/error/error404.html", to_String(_ret));
}