#include "Response.hpp"

std::string Response::get_Current_Time()
{
	char buffer[80];
	std::time_t now = std::time(0);
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T GMT", std::gmtime(&now));
	return (std::string(buffer) + "\r\n");
}

/* ************************************************************************** */

std::string Response::get_Connection()
{
	return (_request.get_connection() + "\r\n");
}

/* ************************************************************************** */

std::string Response::get_Content_Type()
{
	std::string _type;
	size_t i = _path.find_last_of(".");
	if (i == std::string::npos || _ret != 200 || _autoindex)
		return ("text/html \r\n");
	else {
		for (std::map<std::string, std::string>::iterator it = _contentType.begin(); it != _contentType.end(); it++) {
			if ((it->first).compare(_path.substr(i + 1)) == 0) {
				_type = it->second;
			}
		}
		if (_type.empty())
			_type = "application/octet-stream";
		return(_type + "\r\n");
	}
}

/* ************************************************************************** */

std::string	Response::get_Content_Length()
{
	if(_ret == 405)
		return (to_String(_Resbody.length()) + "\r\n");
	return (to_String(_Resbody.length()) + "\r\n\r\n");
}

/* ************************************************************************** */

std::string Response::get_Redirection()
{
	// size_t i;
	// i = server.locations[_l].returned.find(" ");
	// return (server.locations[_l].returned.substr(i).append("\r\n"));
	return (0);
}


/* ************************************************************************** */

/*
std::string Response::get_Allow_Methods()
{
	std::string allowed;
	if (_l != std::string::npos)
	{
		for (std::vector<std::string>::iterator it = server.locations[_l].allow_methods.begin(); \
            it != server.locations[_l].allow_methods.end(); it++) {
			allowed.append(" ").append(*it);
		}
		return (allowed.append("\r\n\r\n"));
	}
	return("");
}
*/

/* ************************************************************************** */

std::string Response::get_cache_control()
{
	return("no-cache\r\n");
}

/* ************************************************************************** */

std::string	Response::to_String(int n)
{
	std::stringstream tmp;

	tmp << n;

	return tmp.str();
}

/* ************************************************************************** */


std::string Response::getcgiheader()
{
	std::string line = "";

	line = cgi_line;

	size_t pos = 0;
	if ((pos = line.find("Content-type")) && pos != std::string::npos)
	{
		int length = strlen("Content-type");
		std::string sub = line.substr(0, pos);
		sub += "Content-Type";
		pos += length;
		sub += line.substr(pos, line.length());
		line = sub;
	}
	return line;
}

/* ************************************************************************** */


std::string Response::setErrorPage(std::string path, std::string status_code)
{
	std::ifstream file;
	file.open(path.c_str());

	std::string line;
	std::string lines;
	std::cout << "error page path " << path  << ", error code " << status_code << std::endl; // TODO remove

	while (getline(file, line))
		lines.append(line + "\n");
	if (path == "./srcs/Conf/error/error.html")
	{
		int pos = lines.find("404");
		lines[pos] = status_code[0];
		lines[pos+1] = status_code[1];
		lines[pos+2] = status_code[2];

		if (_status_code.find(atoi(status_code.c_str())) != _status_code.end())
		{
			pos = lines.find("Not Found");
			int length = strlen("Not Found");
			std::string sub = lines.substr(0, pos);
			sub += _status_code.find(atoi(status_code.c_str()))->second;
			pos += length;
			sub += lines.substr(pos, lines.length());
			lines = sub;
		}
	}
	return lines;
}

/* ************************************************************************** */


int Response::iscgi(std::string path)
{
	if (path.find(".php") != std::string::npos || path.find(".py") != std::string::npos)
	{
		//_l = getIndexLocation(path);
		_l = 0;
		_indexcgi = true;
		//_is_cgi = true;
		return(1);
	}
	return (0);
}



// size_t Response::getIndexLocation(std::string path)
// {
// 	std::string ending;
// 	size_t i = path.find(".");
// 	if (i != std::string::npos){
// 		ending = "*" + path.substr(i);
// 	}
// 	for (size_t i = 0; i != server.locations.size(); i++){
// 		if (!ending.compare(server.locations[i].directive)){
// 			return (i);
// 		}
// 	}
// 	return (SIZE_MAX);
// }