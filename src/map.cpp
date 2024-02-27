#include "webserver.hpp"

// Interface to Parser -> Server
// void map_config_file(std::map<std::string, std::string>& config_map, ConfigParser const &parser_output)
// {
// 	// Tatiana: keep the left side (key) and map the value as from your parser_out as a string

// 	// names based on Tatiana's parser print function:
// 	// server{listen2024;server_namewebserv42.com;body_size10000000;error_page404/error/error404.html;location/{allow_methodsGETPOSTDELETE;autoindexon;indexindex.html;cgi;}

// 	config_map["listen"] = "4432"; // port to be listen for docker
// 	// config_map["listen"] = "4433"; // port to be listen when docker is on
// 	config_map["server_name"] = "webserv42.com";
// 	config_map["body_size"] = "10000000";
// 	config_map["error_page404"] = "/error/error404.html";
// 	config_map["allow_GET"] = "y"; // or n
// 	config_map["allow_POST"] = "y"; // or n
// 	config_map["allow_DELETE"] = "y"; // or n
// 	config_map["autoindex"] = "on"; // or off
// 	config_map["index"] = "html/index.html"; // at the moment index is inside html folder
// 	config_map["cgi-dir"] = "cgi-bin"; // folder to search the cgi files
// 	config_map["limit_conn"] = int2str(SOMAXCONN); // at the max of SOMAXCONN (usually 128 but can be modified)

// 	if (VERBOSE == 1)
// 	{
// 		// print config_map
// 		std::cout << WHITEB << "Printing config_map" << RESET << std::endl;
// 		for (std::map<std::string, std::string>::const_iterator it = config_map.begin(); it != config_map.end(); ++it)
// 			std::cout << "\t" << it->first << " = " << it->second << "; " << std::endl;
// 		std::cout << std::endl;
// 	}
// }

void	map_default_error_pages (std::map<std::string, std::string>& error_page_map)
{
	error_page_map["400"] = "/frontEnd/error/error400.html";
	error_page_map["401"] = "/frontEnd/error/error401.html";
	error_page_map["403"] = "/frontEnd/error/error403.html";
	error_page_map["404"] = "./frontEnd/error/error404.html";
	error_page_map["405"] = "/frontEnd/error/error405.html";
	error_page_map["408"] = "/frontEnd/error/error408.html";
	error_page_map["413"] = "/frontEnd/error/error413.html";
	error_page_map["414"] = "/frontEnd/error/error414.html";
	error_page_map["415"] = "/frontEnd/error/error415.html";
	error_page_map["500"] = "/frontEnd/error/error500.html";
	error_page_map["501"] = "/frontEnd/error/error501.html";
	error_page_map["504"] = "/frontEnd/error/error504.html";
	error_page_map["505"] = "/frontEnd/error/error505.html";
}

// https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
void	map_reponse_status (std::map<std::string, std::string>& response_status_map)
{
	response_status_map["200"] = "OK";
	response_status_map["400"] = "Bad Request";
	response_status_map["401"] = "Unauthorized";
	response_status_map["403"] = "Forbidden";
	response_status_map["404"] = "Not Found";
	response_status_map["405"] = "Method Not Allowed";
	response_status_map["408"] = "Request Timeout";
	response_status_map["413"] = "Request Entity Too Large";
	response_status_map["414"] = "Request-URI Too Long";
	response_status_map["415"] = "Unsupported Media Type";
	response_status_map["500"] = "Internal Server Error";
	response_status_map["501"] = "Not Implemented";
	response_status_map["504"] = "Gateway Timeout";
	response_status_map["505"] = "HTTP Version Not Supported";
}
