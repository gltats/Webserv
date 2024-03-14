/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:24
 * @ Modified by: Gltats
 * @ Modified time: 2024-02-05 17:11:14
 * @ Description: webserv
 */

#include "ConfigParser.hpp"

// Default constructor
ConfigParser::ConfigParser() : _size(0), parameters(), servers(), serverParameters(), listenValues()
{
}

// Copy constructor
ConfigParser::ConfigParser(const ConfigParser &copy)
{
	if (this != &copy)
	{

		this->_size = copy._size;
		this->servers = copy.servers;
		this->parameters = copy.parameters;
		this->serverParameters = copy.serverParameters;
		this->listenValues = copy.listenValues;
	}
}

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

		checkCorrectParameters(parameters);
		serverParameters.push_back(parameters);
	}
	print(); // test function
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
		endPos = content.find("}", startPos);
	}
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
			// Additional checkers for specific parameters
			if (key == "error_page")
			{
				size_t locPos = value.find('/');
				if (locPos != std::string::npos)
				{
					std::string errorNumber = value.substr(0, locPos);
					std::string errorLocation = value.substr(locPos + 1);
					parameters["error_number"] = errorNumber;
					parameters["error_location"] = errorLocation;
				}
			}
			else if (key == "location")
			{

				size_t braceStartPos = value.find('{');
				size_t braceEndPos = value.find('}');

				if (braceStartPos != braceEndPos)
				{
					std::string location = value.substr(0, braceStartPos);
					parameters["location"] = location;
				}
			}
			else if (key == "allow_methods")
			{

				if (value.find("GET") != std::string::npos || value.find("POST") != std::string::npos || value.find("DELETE") != std::string::npos || value.empty())
				{
					if (value.find("GET") != std::string::npos)
					{
						parameters["GET"] = "Get";
					}
					if (value.find("POST") != std::string::npos)
					{
						parameters["POST"] = "Post";
					}
					if (value.find("DELETE") != std::string::npos)
					{
						parameters["DELETE"] = "Delete";
					}
					if (value.empty())
					{
						parameters["NoAllowedMethods"] = "NoAllowedMethods";
					}
				}
				else
				{
					throw std::invalid_argument("Invalid method in allow_methods");
				}

			}
		}
	}
	return parameters;
}

void ConfigParser::checkCorrectParameters(std::map<std::string, std::string> parameters)
{
	std::string listenValue = parameters["listen"];
	std::string serverName = parameters["server_name"];
	std::string bodySize = parameters["body_size"];
	std::string errorNumber = parameters["error_number"];

	if (listenValue.empty() || serverName.empty() || bodySize.empty())
		throw std::invalid_argument("Empty value on configuration file");
	else if (!isDigit(listenValue) || !isDigit(bodySize))
		throw std::invalid_argument("Value is not a digit");
	if (errorNumber != "400" && errorNumber != "401" && errorNumber != "403" && errorNumber != "404" && errorNumber != "405" && errorNumber != "408" && errorNumber != "413" && errorNumber != "414" && errorNumber != "415" && errorNumber != "418" && errorNumber != "500" && errorNumber != "501" && errorNumber != "504" && errorNumber != "505")
	{
		throw std::invalid_argument("Invalid value for 'error_number'");
	}

	listenValues.insert(listenValue);
	listenValues.insert(serverName);
	listenValues.insert(bodySize);
	listenValues.insert(errorNumber);
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
            return "";  // or return some default value
        else
            throw std::invalid_argument("Invalid parameter key");
    }

    return it->second;
}

// Test functions
void ConfigParser::print()
{

	// want to print each part of the config file
	std::cout << "------------- Config File -------------" << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << servers[i] << std::endl;
		std::map<std::string, std::string> parameters = getServerParameters(i);
		std::cout << "listen: " << i << " " << getParameterValue(i, "listen") << std::endl;
		std::cout << "server_name: " << i << " " << getParameterValue(i, "server_name") << std::endl;
		std::cout << "body_size: " << i << " " << getParameterValue(i, "body_size") << std::endl;
		std::cout << "error_page_number: " << i << " " << getParameterValue(i, "error_page") << std::endl;
		std::cout << "location:" << i << " " << getParameterValue(i, "location") << std::endl;
		std::cout << "allowed methods:" << i << " " << getParameterValue(i, "allow_methods") << std::endl;
		std::cout << "autoindex:" << i << " " << getParameterValue(i, "autoindex") << std::endl;
		std::cout << "index:" << i << " " << getParameterValue(i, "indexing") << std::endl;
		std::cout << "scripts:" << i << " " << getParameterValue(i, "cgi") << std::endl;
		// PRINT ERROR PAGES:
		std::cout << "------------- extra -------------" << std::endl;
		std::cout << "error_page_number:" << i << " " << getParameterValue(i, "error_number") << std::endl;
		std::cout << "error_page_number:" << i << " " << getParameterValue(i, "error_location") << std::endl;
		std::cout << "--------------------------------------" << std::endl;
		// PRINT METHODS:
		std::cout << "------------- Methods -------------" << std::endl;
		std::cout << "GET" << i << " " << getParameterValue(i, "GET") << std::endl;
		std::cout << "POST: " << i << " " << getParameterValue(i, "POST") << std::endl;
		std::cout << "DELETE: " << i << " " << getParameterValue(i, "DELETE") << std::endl;
		// if (!parameters["NoAllowedMethods"].empty())
		// {
		// 	std::cout << "NoAllowedMethods: " << parameters["NoAllowedMethods"] << std::endl;
		// }
		if (!getParameterValue(i, "NoAllowedMethods").empty())
		{
			std::cout << "NoAllowedMethods: " << getParameterValue(i, "NoAllowedMethods") << std::endl;
		}
		std::cout << "--------------------------------------" << std::endl;
	}
	std::cout << "--------------------------------------" << std::endl;
}

// getters

int ConfigParser::getSize()
{
	return (this->_size);
}
