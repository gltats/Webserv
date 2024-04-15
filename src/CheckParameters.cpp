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
        bodySizeParam = configParser.getParameterValue(i, "body_size");
        // errorPageParam = configParser.getParameterValue(i, "error_page");
        // errorNumberParam = configParser.getParameterValue(i, "error_number");
        // errorLocationParam = configParser.getParameterValue(i, "error_location");
        if (listenParam.empty() || bodySizeParam.empty())
        {
            throw std::invalid_argument("One or more parameters are missing");
        }
        if (isValidPort(listenParam) == false)
        {
            // std::cout << "Port number is NOT valid: " << listenParam << std::endl; // FOR TESTING
            throw std::invalid_argument("Invalid port number");
        }
        if(isValidBodySize(bodySizeParam) == false)
        {
            throw std::invalid_argument("Invalid body_size number");
            // std::cout << "Body size is NOT valid: " << bodySizeParam << std::endl; // FOR TESTING
        }

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

/*
A common range for client_max_body_size might be between 1m and 100m.
1 megabyte is equal to 1,048,576 bytes (in binary systems, which is typically used in computing).

So, 1 megabyte would be 1 * 1,048,576 bytes, which equals 1,048,576 bytes.
and 100 megabytes would be 100 * 1,048,576 bytes, which equals 104,857,600 bytes.

*/
bool CheckParameters::isValidBodySize(const std::string &bodySize)
{
    for (std::string::const_iterator it = bodySize.begin(); it != bodySize.end(); ++it)
    {
        if (!isdigit(*it))
            return false;
    }
    std::istringstream iss(bodySize);
    int bodySizeNum;
    iss >> bodySizeNum;
    return bodySizeNum >= 1048576 && bodySizeNum <= 104857600;
}