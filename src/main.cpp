#include "ConfigParser.hpp"

int main(int argc, char **argv)
{
    if(argc == 1 || argc == 2)
    {
        std::string		config;
        ConfigParser Configfile;

        config = (argc == 1 ? "configs/default.conf" : argv[1]);
        config.getConfig(config);
		config.print(); // for testing
    }
    else
        std::cout << "Wrong arguments" << std::endl;
}