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
#include <sys/stat.h>
#include <string>
#include <unistd.h>
#include <exception>
#include <vector>
#include <map>
#include <set>

// #include "ServerConfigParser.hpp"

class ConfigParser {
	private:
		std::string		_path;
		size_t			_size;
		std::vector<std::string> servers;
		std::map<std::string, std::string> parameters;
		std::vector<std::map<std::string, std::string> > serverParameters;
		std::set<std::string> listenValues;
	public:
		ConfigParser();
		ConfigParser(std::string const ConfigFile);
		ConfigParser(const ConfigParser &copy);
		ConfigParser &operator=(const ConfigParser &copy);
		~ConfigParser();
		void getConfig(const std::string &configFile);
		std::map<std::string, std::string> parseParameters(const std::string& serverConfig);
		void checkParameters(std::map<std::string, std::string> parameters);
		void splitServers(std::string &content);
		void removeWhiteSpace(std::string& content);
		void removeComments(std::string& content);
		int getTypePath(std::string const path);
		int	checkFile(std::string const path, int mode);
		std::string	readFile(std::string path);
		bool checkExtension(std::string const path);
		bool isFileExistAndReadable(std::string const path, std::string const index);
		bool fileOpen(std::ifstream &configFile);
		void print();
		std::string getPath();
		int getSize();
};