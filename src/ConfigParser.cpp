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
	*this = copy;
}

// copy assignment overload
ConfigParser &ConfigParser::operator=(const ConfigParser &copy)
{
	if (this != &copy)
	{
		
		this->_size = copy._size;
		this->servers = copy.servers;
		this->parameters = copy.parameters;
		this->serverParameters = copy.serverParameters;
		this->listenValues = copy.listenValues;
	}
	return (*this);
}

// Destructor
ConfigParser::~ConfigParser()
{
}

// constructor
// ConfigParser::ConfigParser(std::string const file): _size(0)
// {
// 	std::vector<std::string> servers;
// 	std::map<std::string, std::string> parameters;
// 	std::vector<std::map<std::string, std::string> > serverParameters;
// 	std::set<std::string> listenValues;
// }

////////////////////////////////////////////////////////////////////////////////
// main function:
void ConfigParser::getConfig(const std::string &configtFile)
{
	ConfigFile file(configtFile); 
	// Check if the file exists, has the correct path and is readable
	file.checkPath(configtFile);
	std::string content = file.content;
	removeComments(content);
	removeWhiteSpace(content);
	splitServers(content);

	// Parse the parameters for each server
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
				size_t bracePos = value.find('{');
				if (bracePos != std::string::npos)
				{
					std::string location = value.substr(0, bracePos);
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

	// Check if the "listen" parameter is repeated
	if (listenValues.find(listenValue) != listenValues.end())
	{
		throw std::runtime_error("Error: 'listen' parameter is repeated");
	}
	else if (listenValue.empty() || serverName.empty() || bodySize.empty()) // empty parameters
		throw std::invalid_argument("Empty value on configuration file");
	else if (!std::all_of(listenValue.begin(), listenValue.end(), ::isdigit) || !std::all_of(bodySize.begin(), bodySize.end(), ::isdigit)) // check if the value is a digit
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

void ConfigParser::removeWhiteSpace(std::string &content)
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
		content.erase(pos, endOfLine - pos);
		pos = content.find('#');
	}
}

// Test functions
void ConfigParser::print()
{
	// want to print each part of the config file
	std::cout << "------------- Config File -------------" << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "Server " << i << std::endl;
		std::cout << servers[i] << std::endl;
		// print each parameter
		std::cout << "------------- Parameters -------------" << std::endl;
		std::map<std::string, std::string> parameters = serverParameters[i];
		std::cout << "listen: " << getListenValue(serverParameters[i]) << std::endl;
		std::cout << "server_name: " << getServerName(serverParameters[i]) << std::endl;
		std::cout << "body_size: " << getBodySize(serverParameters[i]) << std::endl;
		std::cout << "error_page_number: " << getErrorNumber(serverParameters[i]) << parameters["error_number"] << std::endl;
		std::cout << "error_location: " << getErrorLocation(serverParameters[i]) << parameters["error_location"] << std::endl;
		std::cout << "location:" << getLocation(serverParameters[i]) << std::endl;

		std::cout << "allowed methods:" << parameters["allow_methods"] << std::endl;
		std::cout << "autoindex:" << parameters["autoindex"] << std::endl;
		std::cout << "index:" << parameters["indexing"] << std::endl;
		std::cout << "scripts:" << parameters["cgi"] << std::endl;
		std::cout << "--------------------------------------" << std::endl;
		// PRINT METHODS:
		std::cout << "------------- Methods -------------" << std::endl;
		std::cout << "GET: " << parameters["GET"] << std::endl;
		std::cout << "POST: " << parameters["POST"] << std::endl;
		std::cout << "DELETE: " << parameters["DELETE"] << std::endl;
		if (!parameters["NoAllowedMethods"].empty())
		{
			std::cout << "NoAllowedMethods: " << parameters["NoAllowedMethods"] << std::endl;
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

std::string ConfigParser::getListenValue(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("listen");
}

std::string ConfigParser::getServerName(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("server_name");
}

std::string ConfigParser::getBodySize(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("body_size");
}

std::string ConfigParser::getErrorNumber(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("error_number");
}

std::string ConfigParser::getErrorLocation(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("error_location");
}

std::string ConfigParser::getLocation(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("location");
}

std::string ConfigParser::getPost(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("POST");
}

std::string ConfigParser::getGet(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("GET");
}

std::string ConfigParser::getDelete(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("DELETE");
}

std::string ConfigParser::getAutoindex(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("autoindex");
}

std::string ConfigParser::getIndexing(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("indexing");
}

std::string ConfigParser::getCgi(const std::map<std::string, std::string> &parameters)
{
	return parameters.at("cgi");
}

