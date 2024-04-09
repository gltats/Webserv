#include "CheckParameters.hpp"

CheckParameters::CheckParameters()
{
}

CheckParameters::CheckParameters(std::string const CheckParameters)
{
    (void)CheckParameters;
}

CheckParameters::CheckParameters(const CheckParameters &copy)
{
    *this = copy;
}

CheckParameters::~CheckParameters()
{
}

void CheckParameters::CheckAllParameters(ConfigParser &configParser)
{

    int serverIndex = configParser.getNumServers();
    // int locationIndex = configParser.getNumLocations(serverIndex);

    for (int i = 0; i < serverIndex; i++)
    {
        listenParam = configParser.getParameterValue(i, "listen");
        // serverNameParam = configParser.getParameterValue(i, "server_name");
        // bodySizeParam = configParser.getParameterValue(i, "body_size");
        // errorPageParam = configParser.getParameterValue(i, "error_page");
        // errorNumberParam = configParser.getParameterValue(i, "error_number");
        // errorLocationParam = configParser.getParameterValue(i, "error_location");
        if (listenParam.empty())
        {
            throw std::invalid_argument("One or more parameters are missing");
        }
        if (isValidPort(listenParam) == false)
        {
            std::cout << "Port number is NOT valid: " << listenParam << std::endl; // FOR TESTING
            // throw std::invalid_argument("Invalid port number");
        }
        else
        {
            std::cout << "Port number is valid: " << listenParam << std::endl; // FOR TESTING
        }

        // for (int j = 0; j < locationIndex; j++)
        // {
        //     locationParam = configParser.getLocationValue(i, j, "location");
        //     autoindexParam = configParser.getLocationValue(i, j, "autoindex");
        //     indexingParam = configParser.getLocationValue(i, j, "indexing");
        //     cgiParam = configParser.getLocationValue(i, j, "cgi");

        // }

        // check if the parameters are correct

    }
}

/*
The port numbers for a localhost (127.0.0.1) can range from 0 to 65535. However, the ports from 0 to 1023 are known as well-known ports and are reserved for privileged services and designated as well-known ports. This means that a regular user process is not allowed to use them for their own services.

Here's a breakdown of the port ranges:

0-1023: Well-known ports. These are allocated to server services by the Internet Assigned Numbers Authority (IANA). e.g., HTTP (80), FTP (21), TELNET (23), etc.
1024-49151: Registered ports. These can be registered for services with the IANA and should be used for custom server services.
49152-65535: Dynamic or private ports. These are usually used for dynamic port allocation and can be used freely by any application.
For development purposes, it's common to use ports in the range of 1024 to 49151. Ports like 3000, 5000, 8000, and 8080 are commonly used for local development.
*/

bool CheckParameters::isValidPort(const std::string &port)
{
    // Check if the port is a default port
    if (port.length() > 7 && port.substr(port.length() - 7) == "default")
    {
        std::string portNumber = port.substr(0, port.length() - 7);
        for (std::string::const_iterator it = portNumber.begin(); it != portNumber.end(); ++it)
        {
            if (!isdigit(*it))
                return false;
        }
        std::istringstream iss(portNumber);
        int portNum;
        iss >> portNum;
        return portNum >= 1024 && portNum <= 49151;
    }

    // Check if the port is a number between 1024 and 49151
    for (std::string::const_iterator it = port.begin(); it != port.end(); ++it)
    {
        if (!isdigit(*it))
            return false;
    }
    std::istringstream iss(port);
    int portNumber;
    iss >> portNumber;
    return portNumber >= 1024 && portNumber <= 49151;
}