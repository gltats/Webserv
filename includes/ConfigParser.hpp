/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:33
 * @ Modified by: Gltats
 * @ Modified time: 2024-04-09 15:44:22
 * @ Description: webserv
 */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <string>
#include <unistd.h>
#include <exception>
#include <vector>
#include <map>
#include <set>
#include <cctype>

#include "ConfigFile.hpp"
#include "CheckParameters.hpp"

class ConfigFile;
class CheckParameters;

static std::string keys[] = {"listen", "server_name", "body_size", "error_page", "location", "allow_methods", "autoindex", "indexing", "cgi"};

class ConfigParser
{
private:
	std::string _path;
	size_t _size;

	std::map<std::string, std::string> parameters;
	std::vector<std::string> servers;
	std::vector<std::map<std::string, std::string> > serverParameters;
	std::vector<std::vector<std::map<std::string, std::string> > > serverLocations;
	std::set<std::string> listenValues;
	

	void checkCorrectParameters(std::map<std::string, std::string> parameters);
	void splitServers(std::string &content);
	void removeWhiteSpace(std::string &content);
	void removeNewLines(std::string &content);
	void removeComments(std::string &content);

	bool isDigit(const std::string &str);
	bool isFileExistAndReadable(std::string const path, std::string const index);
	bool fileOpen(std::ifstream &configFile);

	std::map<std::string, std::string> parseParameters(const std::string &serverConfig);
	std::vector<std::map<std::string, std::string> > parseLocations(const std::string &serverConfig);
	std::map<std::string, std::string> &getServerParameters(size_t index);

	void print();
	std::string getPath();
	
	

public:
	ConfigParser();
	ConfigParser(std::string const ConfigParser);
	ConfigParser(const ConfigParser &copy);
	ConfigParser &operator=(const ConfigParser &copy);
	~ConfigParser();

	void getConfig(const std::string &file);

	std::string getParameterValue(size_t serverIndex, const std::string &parameterKey);
	std::string getLocationValue(size_t serverIndex, size_t locationIndex, const std::string &key);

	size_t getNumServers();
	size_t getNumLocations(size_t serverIndex);
};