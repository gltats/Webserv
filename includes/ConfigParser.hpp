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


class ConfigParser {
	private:
		std::string		_path;
		size_t			_size;
		// std::vector<std::string> servers;
		// std::map<std::string, std::string> parameters;
		// std::vector<std::map<std::string, std::string> > serverParameters;
		// std::set<std::string> listenValues;
	public:
	std::map<std::string, std::string> parameters;
	std::vector<std::string> servers;
	std::vector<std::map<std::string, std::string> > serverParameters;
	std::set<std::string> listenValues;
		ConfigParser();
		// ConfigParser(std::string const ConfigFile);
		ConfigParser(const ConfigParser &copy);
		ConfigParser &operator=(const ConfigParser &copy);
		~ConfigParser();
		void getConfig(const std::string &file);
		std::map<std::string, std::string> parseParameters(const std::string& serverConfig);
		void checkCorrectParameters(std::map<std::string, std::string> parameters);
		void splitServers(std::string &content);
		void removeWhiteSpace(std::string& content);
		void removeComments(std::string& content);
		// int getTypePath(std::string const path);
		// int	checkFile(std::string const path, int mode);
		// std::string	readFile(std::string path);
		// bool checkExtension(std::string const path);
		bool isFileExistAndReadable(std::string const path, std::string const index);
		bool fileOpen(std::ifstream &configFile);
		void print();
		std::string getPath();
		int getSize();
		///getters for sockets
		std::string getListenValue(const std::map<std::string, std::string>& parameters);
		std::string getServerName(const std::map<std::string, std::string>& parameters);
		std::string getBodySize(const std::map<std::string, std::string>& parameters);
		std::string getErrorNumber(const std::map<std::string, std::string> &parameters);
		std::string getErrorPage(const std::map<std::string, std::string> &parameters);
		std::string getErrorLocation(const std::map<std::string, std::string> &parameters);
		std::string getLocation(const std::map<std::string, std::string> &parameters); //NEED TO BE CHANGED
		std::string getPost(const std::map<std::string, std::string> &parameters);
		std::string getGet(const std::map<std::string, std::string> &parameters);
		std::string getDelete(const std::map<std::string, std::string> &parameters);
		std::string getAutoindex(const std::map<std::string, std::string> &parameters);
		std::string getIndexing(const std::map<std::string, std::string> &parameters);
		std::string getCgi(const std::map<std::string, std::string> &parameters);




};