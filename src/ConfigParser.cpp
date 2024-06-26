/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:24
 * @ Modified by: Gltats
 * @ Modified time: 2024-04-15 16:50:54
 * @ Description: webserv
 */

#include "ConfigParser.hpp"
// Default constructor
ConfigParser::ConfigParser() : servers()
{
}

// Copy constructor
ConfigParser::ConfigParser(const ConfigParser &copy)
{
	if (this != &copy)
	{
		this->servers = copy.servers;
	}
}

// Copy assignment overload
ConfigParser &ConfigParser::operator=(const ConfigParser &copy)
{
	if (this != &copy)
	{
		this->servers = copy.servers;
	}
	return (*this);
}

// Constructor with path parameter
// ConfigParser::ConfigParser(std::string const ConfigParser) : _size(0), servers()
// {

// }

// Destructor
ConfigParser::~ConfigParser()
{
}

////////////////////////////////////////////////////////////////////////////////
// main function:
void ConfigParser::getConfig(const std::string &configtFile)
{
	ConfigFile file(configtFile);
	// Check if the file exists, has the correct path and is readable
	file.checkPath(configtFile);
	std::string content = file.content;

	// std::cout << "Heeeeereer Content: " << content << std::endl;
	removeComments(content);
	removeWhiteSpace(content);

	std::istringstream stream(content);
	std::string line;
	while (std::getline(stream, line))
	{
		if (!line.empty())
		{
			char lastChar = line.at(line.size() - 1);
			if (!(line.empty() || lastChar == ';' || lastChar == '{' || lastChar == '}'))
			{
				throw std::invalid_argument("Invalid configuration line");
			}
		}
	}
	removeNewLines(content);

	splitServers(content);
	for (std::vector<std::string>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		parameters = parseParameters(*it);
		locations = parseLocations(*it);
		errorPages = parseErrorPages(*it);

		serverParameters.push_back(parameters);
		serverLocations.push_back(locations);
		serverErrorPages.push_back(errorPages);
	}
	print(); // test function
	CheckParameters parametersChecker;
	parametersChecker.CheckAllParameters(*this);
}

// helper functions
void ConfigParser::splitServers(std::string &content)
{
	size_t startPos = content.find("server{");
	size_t endPos = content.find("}}", startPos);

	if (content.find("server", 0) == std::string::npos)
		throw std::invalid_argument("Server was not found");
	while (startPos != std::string::npos && endPos != std::string::npos)
	{
		if (content.find("server", 0) == std::string::npos)
			throw std::invalid_argument("1 Server found");
		std::string server = content.substr(startPos, endPos - startPos + 1);
		this->servers.push_back(server);

		startPos = content.find("server{", endPos);
		endPos = content.find("}}", startPos);
	}
}

std::vector<std::map<std::string, std::string> > ConfigParser::parseErrorPages(const std::string &serverConfig)
{
	errorPages.clear();
    size_t startPos = serverConfig.find("error_page");
   // std::vector<std::map<std::string, std::string> > errorPages;
    while (startPos != std::string::npos)
    {
		size_t endPos = serverConfig.find(';', startPos);
		std::string errorBlock = serverConfig.substr(startPos, endPos - startPos + 1);

        //std::map<std::string, std::string> errorPageParameters; // Create a new map for each error page
        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
        {
            std::string key = keys[i];
            size_t keyPos = errorBlock.find(key);
            if (keyPos != std::string::npos)
            {
                keyPos += key.length();
				size_t semicolonPos = errorBlock.find(';', keyPos);
				std::string value = errorBlock.substr(keyPos, semicolonPos - keyPos);
                parameters[key] = value;
                // Additional checkers for specific parameters
                size_t locPos = value.find('/');
                if (locPos != std::string::npos)
                {
                    std::string errorNumber = value.substr(0, locPos);
                    std::string errorLocation = value.substr(locPos + 1);
                    errorPageParameters["error_number"] = errorNumber;
                    errorPageParameters["error_location"] = errorLocation;
                }
            }
        }
        errorPages.push_back(errorPageParameters); // Add the parameters map to the errorPages vector
        startPos = serverConfig.find("error_page", endPos); // Find the next "error_pages" occurrence
    }
    return errorPages;
}

std::vector<std::map<std::string, std::string> > ConfigParser::parseLocations(const std::string &serverConfig)
{
	locations.clear();
	size_t startPos = serverConfig.find("location");
	while (startPos != std::string::npos)
	{
		size_t endPos = serverConfig.find('}', startPos);
		std::string locationBlock = serverConfig.substr(startPos, endPos - startPos + 1);

		for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
		{
			std::string key = keys[i];
			size_t keyPos = locationBlock.find(key);
			if (keyPos != std::string::npos)
			{
				keyPos += key.length();
				size_t semicolonPos = locationBlock.find(';', keyPos);
				std::string value = locationBlock.substr(keyPos, semicolonPos - keyPos);
				if (key == "location")
                {
                    size_t bracePos = value.find('{');
                    if (bracePos != std::string::npos)
                    {
                        value = value.substr(0, bracePos);
                    }
                }
				if (key == "allow_methods")
				{
					if (value.find("GET") != std::string::npos || value.find("POST") != std::string::npos || value.find("DELETE") != std::string::npos || value.empty())
					{
						if (value.find("GET") != std::string::npos)
						{
							locationParameters["GET"] = "Get";
						}
						if (value.find("POST") != std::string::npos)
						{
							locationParameters["POST"] = "Post";
						}
						if (value.find("DELETE") != std::string::npos)
						{
							locationParameters["DELETE"] = "Delete";
						}
						if (value.empty())
						{
							locationParameters["NoAllowedMethods"] = "NoAllowedMethods";
						}
					}
					else
					{
						throw std::invalid_argument("Invalid method in allow_methods");
					}

					// Check if value contains any other methods
					std::string allowedMethods = "GETPOSTDELETE";
					size_t pos = value.find_first_not_of(allowedMethods);
					if (pos != std::string::npos)
					{
						throw std::invalid_argument("Invalid method in allow_methods");
					}
				}
				locationParameters[key] = value;
			}
		}

		locations.push_back(locationParameters);

		startPos = serverConfig.find("location", endPos);
	}

	return locations;
}

std::map<std::string, std::string> ConfigParser::parseParameters(const std::string &serverConfig)
{
	for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
	{
		std::string key = keys[i];
		size_t startPos = serverConfig.find(key);
		if (startPos != std::string::npos)
		{
			startPos += key.length();
			size_t endPos = serverConfig.find(';', startPos);
			std::string value = serverConfig.substr(startPos, endPos - startPos);
			parameters[key] = value;
		}
	}
	return parameters;
}

bool ConfigParser::isDigit(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!::isdigit(*it))
		{
			return false;
		}
	}
	return true;
}

void ConfigParser::removeWhiteSpace(std::string &content)
{
	std::string::iterator it = content.begin();
	while (it != content.end())
	{
		if (*it == ' ' || *it == '\t')
			it = content.erase(it);
		else
			++it;
	}
}

void ConfigParser::removeNewLines(std::string &content)
{
	std::string::iterator it = content.begin();
	while (it != content.end())
	{
		if (*it == ' ' || *it == '\t' || *it == '\n' || *it == '\r' || *it == '\f' || *it == '\v')
			it = content.erase(it);
		else
			++it;
	}
}

void ConfigParser::removeComments(std::string &content)
{
	size_t pos = content.find('#'); // Find the first occurrence of '#'

	while (pos != std::string::npos)
	{
		size_t endOfLine = content.find_first_of("\n", pos);
		if (endOfLine != std::string::npos) // If there is a newline after the comment
		{
			content.erase(pos, endOfLine - pos + 1); // Erase the comment and the newline
		}
		else
		{
			content.erase(pos, endOfLine - pos); // If there is no newline, erase to the end of the string
		}
		pos = content.find('#');
	}
}

////////////////////////////////////////////////////////////////////////////////
// the fucking map

std::map<std::string, std::string> &ConfigParser::getServerParameters(size_t index)
{
	if (index >= serverParameters.size())
		throw std::invalid_argument("Invalid server index");

	return serverParameters[index];
}

std::string ConfigParser::getParameterValue(size_t serverIndex, const std::string &parameterKey)
{
	if (serverIndex >= serverParameters.size())
		throw std::invalid_argument("Invalid server index");

	const std::map<std::string, std::string> &parameters = serverParameters[serverIndex];
	std::map<std::string, std::string>::const_iterator it = parameters.find(parameterKey);

	if (it == parameters.end())
	{
		if (parameterKey == "GET" || parameterKey == "POST" || parameterKey == "DELETE" || parameterKey == "NoAllowedMethods")
			return ""; // or return some default value
		else
			throw std::invalid_argument("Invalid method");
	}

	return it->second;
}

std::string ConfigParser::getLocationValue(size_t serverIndex, size_t locationIndex, const std::string &key)
{
	if (serverIndex >= serverLocations.size())
		throw std::invalid_argument("Invalid server index");

	if (locationIndex >= serverLocations[serverIndex].size())
		throw std::invalid_argument("Invalid location index");

	std::map<std::string, std::string> &locationParameters = serverLocations[serverIndex][locationIndex];
	std::map<std::string, std::string>::const_iterator it = locationParameters.find(key);

	if (it == locationParameters.end())
	{
		if (key == "GET" || key == "POST" || key == "DELETE" || key == "NoAllowedMethods")
			return ""; // or return some default value
		else
			throw std::invalid_argument("Invalid parameter key");
	}

	return it->second;
}

std::string ConfigParser::getErrorPageValue(size_t serverIndex, size_t errorIndex, const std::string &key)
{
	if (serverIndex >= serverErrorPages.size())
		throw std::invalid_argument("Invalid server index");

	if (errorIndex >= serverErrorPages[serverIndex].size())
		throw std::invalid_argument("Invalid error index");

	const std::map<std::string, std::string> &errorPageParameters = serverErrorPages[serverIndex][errorIndex];
	std::map<std::string, std::string>::const_iterator it = errorPageParameters.find(key);

	if (it == errorPageParameters.end())
	{
		if (key == "error_page" || key == "error_number" || key == "error_location")
			return ""; // or return some default value
		else
			throw std::invalid_argument("Invalid parameter key");
	}

	return it->second;
}

size_t ConfigParser::getNumServers()
{
	return servers.size();
}

size_t ConfigParser::getNumLocations(size_t serverIndex)
{
	if (serverIndex >= serverLocations.size())
		throw std::invalid_argument("Invalid server index");

	return serverLocations[serverIndex].size();
}

size_t ConfigParser::getNumErrorPages(size_t serverIndex)
{
	if (serverIndex >= serverErrorPages.size())
		throw std::invalid_argument("Invalid server index");

	return serverErrorPages[serverIndex].size();
}

// Test function
void ConfigParser::print()
{

	// want to print each part of the config file
	std::cout << "======================================== Config File ========================================" << std::endl;
	for (size_t i = 0; i < getNumServers(); i++)
	{
		std::cout << servers[i] << std::endl;
		// std::map<std::string, std::string> parameters = getServerParameters(i);
		// std::cout << "************************ Main *****************************" << std::endl;
		// std::cout << "listen: " << i << " " << getParameterValue(i, "listen") << std::endl;
		// std::cout << "server_name: " << i << " " << getParameterValue(i, "server_name") << std::endl;
		// std::cout << "body_size: " << i << " " << getParameterValue(i, "body_size") << std::endl;
		// std::cout << "***********************************************************" << std::endl;
		// std::cout << "********************** Error pages **************************" << std::endl;
		// for (size_t x = 0; x < serverErrorPages[i].size(); x++)
		// {
		// 	std::map<std::string, std::string> errorPageParameters = serverErrorPages[i][x];
		// 	// std::cout << x << " error page on server " << i << ": " << getErrorPageValue(i, x, "error_page") << std::endl;
		// 	std::cout << x << " error page number on server " << i << ": " << getErrorPageValue(i, x, "error_number") << std::endl;
		// 	std::cout << x << " error page location on server " << i << ": " << getErrorPageValue(i, x, "error_location") << std::endl;
		// }
		// std::cout << "" << std::endl;
		// std::cout << "***********************************************************" << std::endl;
		// std::cout << "********************** Locations **************************" << std::endl;
		for (size_t j = 0; j < serverLocations[i].size(); j++)
		{
			std::map<std::string, std::string> locationParameters = serverLocations[i][j];
			std::cout << j << " location on server " << i << ": " << getLocationValue(i, j, "location") << std::endl;
			std::cout << j << " location with allowed methods on server " << i << ": " << getLocationValue(i, j, "GET") << " " << getLocationValue(i, j, "POST") << " " << getLocationValue(i, j, "DELETE") << std::endl;
			if (!getLocationValue(i, j, "NoAllowedMethods").empty())
			{
				std::cout << "NoAllowedMethods" << j << ": " << getLocationValue(i, j, "NoAllowedMethods") << std::endl;
			}
			std::cout << j << " location with autoindex on server " << i << ": " << getLocationValue(i, j, "autoindex") << std::endl;
			std::cout << j << " location with cgi on server " << i << ": " << getLocationValue(i, j, "cgi") << std::endl;
		}
		// std::cout << "" << std::endl;
		// std::cout << "***********************************************************" << std::endl;
	}
	// std::cout << "=============================================================================================" << std::endl;
}