#include "Response.hpp"

int Response::redirection()
{
    std::stringstream nb;
    size_t i;
    if (!_server.getLocationValue(0, 0, "location").empty())
    {
        i = _server.getLocationValue(0, 0, "location").find(" ");
        nb << _server.getLocationValue(0, 0, "location").substr(0, i);
        nb >> _ret;
        return(1);
    }
    return(0);
}