#include "Response.hpp"


void Response::error()
{
    // Get the number of error pages for the server
    size_t numErrorPages = _server.getNumErrorPages(_serverID);
    std::cout << "Number of error pages: " << numErrorPages << std::endl;

    // Loop through each error page
    for (size_t i = 0; i < numErrorPages; i++)
    {
        // Get the error number for the current error page
        int errorNumber = str2int(_server.getErrorPageValue(_serverID, i, "error_number"));
        std::cout << "Error number for page " << i << ": " << errorNumber << std::endl;

        // If the return code matches the error number
        if (_ret == errorNumber)
        {
            // Build the error page path
            std::string error_page_path = ROOT;
            error_page_path.append("/");
            error_page_path.append(_server.getErrorPageValue(_serverID, i, "error_location"));
            std::cout << "Error page path: " << error_page_path << std::endl;

            // Set the response body to the error page
            _Resbody = setErrorPage(error_page_path, to_String(_ret));
            return;
        }
    }

    // If no user-defined error page was found, use the default ones
    std::string error_page_path = "./frontEnd/error/error" + int2str(_ret) + ".html";
    std::cout << "Default error page path: " << error_page_path << std::endl;

    // Check if the error page exists
    if (access(error_page_path.c_str(), R_OK) == 0)
    {
        // Set the response body to the error page
        _Resbody = setErrorPage(error_page_path, to_String(_ret));
        return;
    }
    else
    {
        // If the error page does not exist, print an error message
        std::cerr << REDB << "Error page :" << error_page_path << ", does not exist or not accessible" << std::endl;
    }

    // If no error page was found, use the default error
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