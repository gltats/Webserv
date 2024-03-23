/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:33
 * @ Modified by: Gltats
 * @ Modified time: 2024-02-05 17:07:40
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

class ConfigFile;

static std::string keys[] = {"listen", "server_name", "body_size", "error_page", "location", "allow_methods", "autoindex", "indexing", "cgi"};

class ConfigParser
{
private:
	std::string _path;
	size_t _size;

public:
	std::map<std::string, std::string> parameters;
	std::vector<std::string> servers;
	std::vector<std::map<std::string, std::string> > serverParameters;
	std::vector<std::vector<std::map<std::string, std::string> > > serverLocations;
	std::set<std::string> listenValues;
	ConfigParser();
	ConfigParser(std::string const ConfigParser);
	ConfigParser(const ConfigParser &copy);
	ConfigParser &operator=(const ConfigParser &copy);
	~ConfigParser();
	void getConfig(const std::string &file);
	std::map<std::string, std::string> parseParameters(const std::string &serverConfig);
	std::vector<std::map<std::string, std::string> > parseLocations(const std::string &serverConfig);
	
	bool isDigit(const std::string &str);

	void checkCorrectParameters(std::map<std::string, std::string> parameters);
	void splitServers(std::string &content);
	void removeWhiteSpace(std::string &content);
	void removeNewLines(std::string &content);
	void removeComments(std::string &content);


	bool isFileExistAndReadable(std::string const path, std::string const index);
	bool fileOpen(std::ifstream &configFile);
	void print();
	std::string getPath();
	int getSize();
	std::map<std::string, std::string> &getServerParameters(size_t index);
	std::string getParameterValue(size_t serverIndex, const std::string &parameterKey);
	std::string getLocationValue(size_t serverIndex, size_t locationIndex, const std::string &key);


	void setSize(int nb); // TODO remove : from Maira
};