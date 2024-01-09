/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:24
 * @ Modified by: Gl.tats
 * @ Modified time: 2024-01-09 17:13:19
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

//copy assignment overload
ConfigParser &ConfigParser::operator=(const ConfigParser &copy) {
	if (this == &copy)
		return *this;
}

//Destructor
ConfigParser::~ConfigParser()
{}

//constructor
ConfigParser::ConfigParser(std::string const path):  _path(path), _size(0), _nb_server(0)
{}

////////////////////////////////////////////////////////////////////////////////
//main function:
void ConfigParser::getConfig(const std::string &configFile)
{
	std::string		content;
	// Check if the file exists and is readable
    int fileType = getTypePath(configFile);
    if (fileType == -1) {
        throw FileNotOpen; // Error in getTypePath
    } else if (fileType != 1) {
        throw FileIsNotFile; // Not a regular file
    } else if (checkFile(configFile, 4) != 0) {
        throw FileIsUnaccessible; // File not readable
    }
	//try to open file
	content = readFile(configFile);
	if (content.empty())
		throw FileIsEmpty;
	removeComments(content);
	removeWhiteSpace(content);
	splitServers(content);//spliting servers on separetly strings in vector (learning vector), later I have to add a checker for checking all the value requiered are there and they all have the ;
	return (0);
}



//helper functions
void ConfigParser::removeWhiteSpace(const std::string& content)
{
	std::string::iterator it = content.begin();
	while (it != content.end()) {
		if (*it == ' ' || *it == '\t' || *it == '\n' || *it == '\r' || *it == '\f' || *it == '\v')
			it = content.erase(it);
		else
			++it;
	}
}

void ConfigParser::removeComments(const std::string& content)
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

int	ConfigParser::checkFile(std::string const path, int mode)
{
	return (access(path.c_str(), mode));
}

// reading from file to string
std::string	ConfigParser::readFile(std::string path)
{
	if (path.empty() || path.length() == 0)
		throw PathIssue;
	std::ifstream config_file(path.c_str());
	if (!config_file || !config_file.is_open())
		throw FileNotOpen;

	std::stringstream stream_binding;
	stream_binding << config_file.rdbuf();
	return (stream_binding.str());
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


//failure strings
const char	*ConfigParser::BracketsNotClosed::what() const throw() {
	return "Brackets for the block are not closed.";
}

const char	*ConfigParser::BlocknameNotExisting::what() const throw() {
	return "This blockname doesnt exist in the provide config file";
}

const char	*ConfigParser::NoSemicolonAtTheEndOfContext::what() const throw() {
	return "The context has no semicolon at the end of it";
}

const char	*ConfigParser::FileNotOpen::what() const throw(){
	return "Could not open the config file.";
}

const char	*ConfigParser::LocationAlreadyExists::what() const throw() {
	return "Location already exists.";
}

const char	*ConfigParser::ContextExistsMoreThanOnce::what() const throw() {
	return "Context already exits for this block.";
}

const char	*ConfigParser::FileIsUnaccessible::what() const throw() {
	return "File is not accesible";
}

const char	*ConfigParser::FileIsEmpty::what() const throw() {
	return "File is empty";
}

const char	*ConfigParser::SizeError::what() const throw() {
	return "Size error";
}

const char *ConfigParser::FileIsNotFile::what() const throw() {
	return "Your are trying to open something that is not a normal file, maybe a folder"
}

const char *ConfigParser::PathIssue::what() const throw() {
	return "The Path provided is incorrect"
}

//Test functions
int ConfigParser::print()
{
	//want to print each part of the config file
	std::cout << "------------- Config File -------------" << std::endl;
	

}

//getters
std::string ConfigFile::getPath()
{
	return (this->_path);
}

int ConfigFile::getSize()
{
	return (this->_size);
}
