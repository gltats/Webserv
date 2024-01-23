#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP


#include <map>
#include <iostream>
#include <signal.h>

#include "color_code.hpp"

#include "ConfigParser.hpp"
#include "Server.hpp"
#include "Connection.hpp"


void 	map_config_file(std::map<std::string, std::string>& config_map, ConfigParser const &parser_output);
void	map_default_error_pages (std::map<std::string, std::string>& error_page_map);

void	ft_signal_handler(int signum);
void	ft_setup_sighandler(void);

#endif