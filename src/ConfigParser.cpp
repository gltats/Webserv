/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:24
 * @ Modified by: Gl.tats
 * @ Modified time: 2024-01-03 16:46:20
 * @ Description: webserv
 */

#include "ConfigParser.hpp"

//Default constructor
ConfigParser::ConfigParser(): _nbServer(0)
{}

// Copy constructor
ConfigParser(const ConfigParser &copy)
{
	*this = copy;
}

//copy assignment overload
ConfigParser &ConfigParser::operator=(const ConfigParser &copy) {
	if (this == &copy)
		return *this;
}

//Destructor
ConfigParser::ConfigParser()
{}

////////////////////////////////////////////////////////////////////////////////
//functions:

void ConfigParser::getConfig(const std::string &config_file)
{
 	//checking and read config file,
}

std::string cleanConfig(const std::string& content)
{
	//clean file from spaces and empty lines
}

int ConfigParser::print()
{
	//want to print each part of the config file
	std::cout << "------------- Config File -------------" << std::endl;

}