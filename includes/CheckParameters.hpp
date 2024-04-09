#pragma once

#include <iostream>
#include <fstream>


#include "ConfigParser.hpp"
#include "CheckParameters.hpp"

class ConfigParser;
class CheckParameters
{
    private:
        std::string listenParam;
        std::string serverNameParam;
        std::string bodySizeParam;
        std::string errorPageParam;
        std::string errorNumberParam;
        std::string errorLocationParam;
        std::string locationParam;
        std::string autoindexParam;
        std::string indexingParam;
        std::string cgiParam;
        bool isValidPort(const std::string &port);

    public:
        CheckParameters();
        CheckParameters(std::string const CheckParameters);
        CheckParameters(const CheckParameters &copy);
        ~CheckParameters();
        void CheckAllParameters(ConfigParser &configParser);

};