/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:24
 * @ Modified by: Gltats
 * @ Modified time: 2024-02-01 17:40:34
 * @ Description: webserv
 */

#include "ConfigParser.hpp"

//Default constructor
ConfigParser::ConfigParser(): _size(0)
{}

// Copy constructor
ConfigParser::ConfigParser(const ConfigParser &copy)
{
	*this = copy;
}

// copy assignment overload
ConfigParser &ConfigParser::operator=(const ConfigParser &copy) {
	if (this != &copy)
	{
		this->_path = copy._path;
		this->_size = copy._size;
	}
	return (*this);
}

//Destructor
ConfigParser::~ConfigParser()
{}

//constructor
ConfigParser::ConfigParser(std::string const ConfigFile):  _path(ConfigFile), _size(0)
{
	std::vector<std::string> servers;
}

////////////////////////////////////////////////////////////////////////////////
//main function:
void ConfigParser::getConfig(const std::string &configFile)
{
	std::string		content;
	// Check if the file exists, has the correct path and is readable
    int fileType = getTypePath(configFile);
    if (fileType == -1) {
        throw std::invalid_argument("File is not open");// Error in getTypePath
    } else if (fileType != 1) {
        throw std::invalid_argument("File is not a regular file");; // Not a regular file
    } else if (checkFile(configFile, 4) != 0) {
        throw  std::invalid_argument("File not redable"); // File not readable
    } else if (!checkExtension(configFile)) 
	{
		throw std::invalid_argument("File has wrong extension"); // Wrong extension
	}
	//try to open file
	content = readFile(configFile);
	if (content.empty())
		throw std::invalid_argument("File is empty");
	removeComments(content);
	removeWhiteSpace(content);
	splitServers(content);

    // Parse the parameters for each server
  	for (std::vector<std::string>::iterator it = servers.begin(); it != servers.end(); ++it) 
	{
        std::map<std::string, std::string> parameters = parseParameters(*it);

		// Check if the "listen" parameter is repeated
		std::string listenValue = parameters["listen"];
        if (listenValues.find(listenValue) != listenValues.end()) {
            throw std::runtime_error("Error: 'listen' parameter is repeated");
        }
		listenValues.insert(listenValue);

		serverParameters.push_back(parameters);
		
    }
	print();// test function
}

//helper functions
void ConfigParser::splitServers(std::string &content)
{
    size_t startPos = content.find("server{");
    size_t endPos = content.find("}", startPos);

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

std::map<std::string, std::string> ConfigParser::parseParameters(const std::string& serverConfig) 
{
    std::map<std::string, std::string> parameters;
    std::string keys[] = {"listen", "server_name", "body_size", "error_page", "location", "allow_methods", "autoindex", "index", "cgi"};

    for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
        std::string key = keys[i];
        size_t startPos = serverConfig.find(key);
        if (startPos != std::string::npos) {
            startPos += key.length();
            size_t endPos = serverConfig.find(';', startPos);
            std::string value = serverConfig.substr(startPos, endPos - startPos);
            parameters[key] = value;
        }
    }
    return parameters;
}

void ConfigParser::removeWhiteSpace(std::string& content)
{
    std::string::iterator it = content.begin();
    while (it != content.end()) {
        if (*it == ' ' || *it == '\t' || *it == '\n' || *it == '\r' || *it == '\f' || *it == '\v')
            it = content.erase(it);
        else
            ++it;
    }
}

void ConfigParser::removeComments(std::string& content)
{
    size_t pos = content.find('#'); // Find the first occurrence of '#'

    while (pos != std::string::npos)
    {
        size_t endOfLine = content.find_first_of("\n", pos);
        content.erase(pos, endOfLine - pos);
        pos = content.find('#');
    }
}

//Check if the the path is a file, a forder or something else
int ConfigParser::getTypePath(std::string const path)
{
	struct stat	buffer;
	int			result;
	
	result = stat(path.c_str(), &buffer);
	if (result == 0)
	{
		if (buffer.st_mode & S_IFREG)
			return (1);
		else if (buffer.st_mode & S_IFDIR)
			return (2);
		else
			return (3);
	}
	else
		return (-1);
}

//Check if the file exists and is readable
int	ConfigParser::checkFile(std::string const path, int mode)
{
	return (access(path.c_str(), mode));
}

// reading from file to string
std::string	ConfigParser::readFile(std::string path)
{
	if (path.empty() || path.length() == 0)
		throw std::invalid_argument("Path is wrong");
	std::ifstream config_file(path.c_str());
	if (!config_file || !config_file.is_open())
		throw std::invalid_argument("File is not open");

    std::string content((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());
    return content;
}

//Check if extension is correct
bool ConfigParser::checkExtension(std::string const path)
{
	size_t pos = path.find(".conf");
	if (pos == std::string::npos)
		return (false);
	return (true);
}


bool ConfigParser::isFileExistAndReadable(std::string const path, std::string const index)
{
	if (getTypePath(index) == 1 && checkFile(index, 4) == 0)
		return(true);
	if (getTypePath(path + index) == 1 && checkFile(path + index, 4) == 0)
		return(true);
	return(false);
}

bool	ConfigParser::fileOpen(std::ifstream &configFile) {
	if (!configFile.is_open())
		return false;
	return true;
}



//Test functions
void ConfigParser::print()
{
	//want to print each part of the config file
	std::cout << "------------- Config File -------------" << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "Server " << i << std::endl;
		std::cout << servers[i] << std::endl;
		//print each parameter
		std::cout << "------------- Parameters -------------" << std::endl;
		std::map<std::string, std::string> parameters = serverParameters[i];
        std::cout << "listen: " << parameters["listen"] << std::endl;
        std::cout << "server_name: " << parameters["server_name"] << std::endl;
        std::cout << "body_size: " << parameters["body_size"] << std::endl;
        std::cout << "error_page: " << parameters["error_page"] << std::endl;
		std::cout << "--------------------------------------" << std::endl;
		
	}
	std::cout << "--------------------------------------" << std::endl;
}

//getters
std::string ConfigParser::getPath()
{
	return (this->_path);
}

int ConfigParser::getSize()
{
	return (this->_size);
}
