#pragma once

# include <iostream>
# include <string>
# include <exception>

# include "ConfigParser.hpp"


class ConfigParser;
class ConfigFile
{
    public:
        ConfigFile();
        ConfigFile(std::string const ConfigFile);
        ConfigFile(const ConfigFile &copy);
        ConfigFile &operator=(const ConfigFile &copy);
        ~ConfigFile();
        void checkPath(std::string const path);
        std::string content;
    private:
        std::string _path;
        int getTypePath(std::string const path);
        int	checkFile(std::string const path, int mode);
        std::string	readFile(std::string path);
        bool checkExtension(std::string const path);
        bool isFileExistAndReadable(std::string const path, std::string const index);
        bool fileOpen(std::ifstream &configFile);

};