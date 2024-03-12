
// MOCK Config Parser
#include "ConfigParser.hpp"
#include <iostream>

//server{listen2024;server_namewebserv42.com;body_size10000000;error_page404/error/error404.html;location/{allow_methodsGETPOSTDELETE;autoindexon;indexindex.html;cgi;}

ConfigParser::ConfigParser(void)
{
    // server 1:
    _server_map[0]["listen"] = "4432";
    _server_map[0]["server_name"] = "webserv42.com";
    _server_map[0]["body_size"] = "10000000";
    _server_map[0]["error_page404"] = "/error/error404.html";
    _server_map[0]["location"] = "/";
    _server_map[0]["allow_GET"] = "y";
    _server_map[0]["allow_POST"] = "y";
    _server_map[0]["allow_DELETE"] = "y";
    _server_map[0]["autoindex"] = "on";
    _server_map[0]["index"] = "index.html";
    _server_map[0]["cgi"] = "";

    // server 2:
    _server_map[1]["listen"] = "4433";
    _server_map[1]["server_name"] = "devserver.de";
    _server_map[1]["body_size"] = "500";
    _server_map[1]["error_page404"] = "/error/error404.html";
    _server_map[1]["location"] = "/";
    _server_map[1]["allow_GET"] = "y";
    _server_map[1]["allow_POST"] = "n";
    _server_map[1]["allow_DELETE"] = "y";
    _server_map[1]["autoindex"] = "off";
    _server_map[1]["index"] = "basic_server2.html";
    _server_map[1]["cgi"] = "";

    // server 3:
    _server_map[2]["listen"] = "4432";
    _server_map[2]["server_name"] = "";
    _server_map[2]["body_size"] = "100";
    _server_map[2]["error_page404"] = "/error/error404.html";
    _server_map[2]["location"] = "/";
    _server_map[2]["allow_GET"] = "y";
    _server_map[2]["allow_POST"] = "y";
    _server_map[2]["allow_DELETE"] = "n";
    _server_map[2]["autoindex"] = "on";
    _server_map[2]["index"] = "basic_server3.html";
    _server_map[2]["cgi"] = "";


    _nb_servers = 3;
}

ConfigParser::~ConfigParser(void)
{

}

std::string ConfigParser::get_server_map_key(int server_id, std::string key)
{
    return(_server_map[server_id][key]);
}

int         ConfigParser::get_nb_of_servers(void) const
{
    return (_nb_servers);
}

std::string ConfigParser::get_listen(int server_id) const
{
    return _server_map.at(server_id).at("listen");
}

std::string ConfigParser::get_server_name(int server_id) const
{
    return _server_map.at(server_id).at("server_name");
}

std::string ConfigParser::get_error_page404(int server_id) const
{
    return _server_map.at(server_id).at("error_page404");
}

std::string ConfigParser::get_allow_GET(int server_id) const
{
    return _server_map.at(server_id).at("allow_GET");
}

std::string ConfigParser::get_allow_POST(int server_id) const
{
    return _server_map.at(server_id).at("allow_POST");
}

std::string ConfigParser::get_allow_DELETE(int server_id) const
{
    return _server_map.at(server_id).at("allow_DELETE");
}

std::string ConfigParser::get_allow_autoindex(int server_id) const
{
    return _server_map.at(server_id).at("autoindex");
}
 
std::string ConfigParser::get_allow_index(int server_id) const
{
    return _server_map.at(server_id).at("index");
}

std::string ConfigParser::get_allow_cgi(int server_id) const
{
    return _server_map.at(server_id).at("cgi");
}
