#include "ConfigFile.hpp"


// Default constructor
ConfigFile::ConfigFile()
{
}

// Copy constructor
ConfigFile::ConfigFile(const ConfigFile &copy)
{
	*this = copy;
}

// copy assignment overload
ConfigFile &ConfigFile::operator=(const ConfigFile &copy)
{
	if (this != &copy)
	{
        this->_path = copy._path;
    }
	return (*this);
}

// Destructor
ConfigFile::~ConfigFile()
{
}

// constructor
ConfigFile::ConfigFile(std::string const ConfigFile): _path(ConfigFile)
{
    checkPath(ConfigFile);
}

void ConfigFile::checkPath(std::string const path)
{
    // std::string content;
    int fileType = getTypePath(path);
    
    if (fileType == -1)
	{
		throw std::invalid_argument("File is not open"); // Error in getTypePath
	}
	else if (fileType != 1)
	{
		throw std::invalid_argument("File is not a regular file");
		; // Not a regular file
	}
	else if (checkFile(path, 4) != 0)
	{
		throw std::invalid_argument("File not redable"); // File not readable
	}
	else if (!checkExtension(path))
	{
		throw std::invalid_argument("File has wrong extension"); // Wrong extension
	}
	// try to open file
	content = readFile(path);
	if (content.empty())
		throw std::invalid_argument("File is empty");
}


// Check if the the path is a file, a forder or something else
int ConfigFile::getTypePath(std::string const path)
{
	struct stat buffer;
	int result;

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

// Check if the file exists and is readable
int ConfigFile::checkFile(std::string const path, int mode)
{
	return (access(path.c_str(), mode));
}

// Check if extension is correct
bool ConfigFile::checkExtension(std::string const path)
{
	size_t pos = path.find(".conf");
	if (pos == std::string::npos)
		return (false);
	return (true);
}

// reading from file to string
std::string ConfigFile::readFile(std::string path)
{
	if (path.empty() || path.length() == 0)
		throw std::invalid_argument("Path is wrong");
	std::ifstream config_file(path.c_str());
	if (!config_file || !config_file.is_open())
		throw std::invalid_argument("File is not open");

	std::string content((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());
	return content;
}

bool ConfigFile::isFileExistAndReadable(std::string const path, std::string const index)
{
	if (getTypePath(index) == 1 && checkFile(index, 4) == 0)
		return (true);
	if (getTypePath(path + index) == 1 && checkFile(path + index, 4) == 0)
		return (true);
	return (false);
}

bool ConfigFile::fileOpen(std::ifstream &configFile)
{
	if (!configFile.is_open())
		return false;
	return true;
}

std::string ConfigParser::getPath()
{
	return (this->_path);
}
