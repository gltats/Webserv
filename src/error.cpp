#include "Response.hpp"


void Response::error()
{
	// only supporting one error_page being set by the user or the default ones

	// for (size_t i = 0; i < _server.error_page.size(); i++) //
	// {
		// if (_ret == _server.error_page[i].status_code) // original
				// spError(server.error_page[i].page);  // original

		// this get's the error code defined by the user in the configuration file
		std::cout << REDB << "Finding an error page :" << _ret << RESET << std::endl;
		if (_ret == str2int(_server.getParameterValue(_serverID, "error_number"))) // modified maira
		{
			_Resbody = setErrorPage(_server.getParameterValue(_serverID, "error_location"), to_String(_ret));
			return ;
		}

		// if user didnt define one, use the defaults.
		else
		{
			// build servers predefined errors pages path
			std::string error_page_path = "./frontEnd/error/error" + int2str(_ret) + ".html";

			// check if exist
			if (access(error_page_path.c_str(), R_OK) == 0)
			{
				_Resbody = setErrorPage(error_page_path, to_String(_ret));
				return ;
			}
			else
			{
				std::cerr << REDB << "Error page :" << error_page_path << ", does not exist or not accessable" << std::endl;
				// return the default one in defError
			}
		// }
	}
	defError();
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