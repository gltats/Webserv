#ifndef MAP_HPP
#define MAP_HPP

#include <map>
#include "ConfigParser.hpp"

// void 	map_config_file(std::map<std::string, std::string>& config_map, ConfigParser const &parser_output);
void	map_default_error_pages (std::map<std::string, std::string>& error_page_map);
void	map_reponse_status (std::map<std::string, std::string>& response_status_map);


#endif