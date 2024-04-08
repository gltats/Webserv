/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 21:53:38 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/08 17:32:50 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"



// Response::Response(ConfigParser &configParser, Request &request, char *env[]): _configParser(configParser), _request(request), _envp(env)
// {
// 	// to avoid error of unused argumentss
// 	if (_configParser.getParameterValue(0, "listen").length() != 0 || _request.get_method().compare("Hi") == 0 || env == 0)
// 		std::cout << "";


// 	_html_content_size = 0;
// 	_html_content.clear();
// 	_response.clear();


// 	std::cout << "Response default constructor" << std::endl;


// }

// /*
// 	 "HTTP/1.1 200 OK\r\nContent-Length: 88\r\nContent-Type: text/html\r\n\r\n
// 		cgi file must be chmod 755 hello.py
// */

// // Response::Response(Response const &src)
// // {
// // 	(void)src;
// // }

// Response	&Response::operator=(Response const &rhs)
// {
// 	(void)rhs;
// 	return (*this);
// }

void	Response::_setup_response(void)
{
	// std::cout << "Response parametric constructor" << std::endl;
	_html_content_size = 0;
	_html_content.clear();
	_status_line.clear();
	_response.clear();
	_is_cgi = 0;
}

// // void	Response::set_error_page_map(std::map<std::string, std::string> *error_page_map)
// // {
// // 	_error_page_map = error_page_map;
// // }

// // std::string	Response::get_response(void) const
// // {
// // 	return (_response);
// // }

void	Response::create_response(int server_id)
{
	_serverID = server_id;
	// initErrorMap(); // moved to Response constructor
	// initRespMaps(); // moved to Response constructor
	// initContentMap(); // moved to Response constructor
	_setup_response(); // move to this line, it cleans attributes before new message is received
	setPath();
	setBool();
	if (isAbsoluteURI())
		changeHost();
	call(); // added function to create a response
	//_parse_response(_request);
	
}

std::string		Response::get_response(void) const
{
	return (_response);
}



void Response::process_cgi(char const *buffer, int buffer_size)
{
	if (buffer_size == 0 && buffer[0] != 'c') // consume as it is not used at the moment
		std::cout << "";
// 	// dup2(_response.get_fd_stdin(), STDIN_FILENO); 
// 	// create a response
// 	// close(_ fd_pipe[0]);
// 	_create_status_line();

// 	// append response
// 	_response.append(_status_line);
// 	_response.append("Server: Webserv\r\n");

// 	_response.append(buffer);
}

// void	Response::_parse_response(Request const &req)
// {
// 	//  _response ="HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: Apache/2.2.14 (Win32)\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 88\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html><body><h1>Hello, World9</h1></body></html>";
// 	_is_cgi = false;
// 	// check if it is a cgi
// 	std::string uri = req.get_uri();
// 	if (uri.find("cgi-bin/") != std::string::npos)
// 	{
// 			_is_cgi = true;
// 		_fd_stdin = dup(STDIN_FILENO); // save original stdin
// 		_fd_stdout = dup(STDOUT_FILENO); // save original stdin

// 		if (pipe(_fd_pipe) == -1)
// 		{
// 			// error
// 			std::cout << "Error: pipe creation" << std::endl;
// 			return ;
// 		}
// 		std::cout << "pipe0 is fd" << _fd_pipe[0] << std::endl; //remove 
// 		// it is a cgi
// 		pid_t pid;
// 		pid = fork();
// 		if (pid == 0)
// 		{
// 			std::cout << "...Trying to run CGI : " << uri << std::endl;

// 			// std::string	python_path;
// 			char *python_path;

// 			char python_path_linux[] = "/usr/bin/python3";
// 			char python_path_mac[] = "/usr/local/bin/python3";

// 			python_path = python_path_mac;
// 			if (OS_PATH == LINUX)
// 				python_path = python_path_linux;


		
// 			uri.erase(0, 1);
// 			char *exe[3] = {python_path, (char *)uri.c_str(), 0};

// 			dup2(_fd_pipe[1], STDOUT_FILENO);
// 			close(_fd_pipe[0]);
// 			close(_fd_pipe[1]);
// 			close(_fd_stdin);

// 			execve(exe[0], exe, _envp);
// 			std::cout << REDB << "reason:" << strerror(errno) <<  RESET << std::endl;

// 			dup2(_fd_stdout, STDOUT_FILENO); // return output to stdout to print error
// 			std::cout << "Error: CGI could not be executed : " << uri << std::endl;
// 			exit(1);
// 		}
// 		else
// 		{
// 			waitpid(pid, 0, WNOHANG);
// 			close(_fd_pipe[1]);
// 			close (_fd_stdin);
// 			close(_fd_stdout);
// 			return ;
// 		}

// 	}

// 	if (_read_file_data(req) == -1)
// 		return ;

// 	// create a response
// 	_create_status_line();

// 	// // append response
// 	_response.append(_status_line);

// 	// content length
// 	std::stringstream	ss_len;

// 	ss_len << "Server: Webserv" << "\r\n";

// 	ss_len << "Content-Length: " << _html_content_size +1  << "\r\n";
// 	ss_len << "Content-Type: " << "text/html"  << "\r\n";

// 	_response.append(ss_len.str()); // convert string stream to a string

// 	_response.append("\r\n"); // empty line
// 	_response.append(_html_content);
// 	// std::cout << "file content is size " << _html_content_size << " (" << int2str(_html_content_size) << ")" << std::endl;
// }


		
bool		Response::get_is_cgi(void)
{
	return (_is_cgi);
}


int	Response::get_fd_stdin(void) const
{
	return (_fd_stdin);
}


int	Response::get_fd_pipe_0(void) const
{
	return (_fd_pipe[0]);
}


// Response::~Response(void)
// {

// 	std::cout << "Response deconstructor" << std::endl;

// }

// #include <fcntl.h>
// int	Response::_read_file_data(Request const &req)
// {
// 	std::ifstream 		html_file;
// 	std::stringstream	ss;
// 	std::string file = ".";
// 	file.append(req.get_uri());
// 	// std::string			fcontent;

// 	// std::cout << "...trying to open <" <<req.get_uri() << ">" << std::endl;

// 	html_file.open(file.c_str(), std::ifstream::in);
// 	if (html_file.is_open() == 0)
// 	{
// 		std::cout << REDB << "Error: _get_file_data. File could not be openned" << RESET << std::endl;

// 		// at the moment the path to the style and img are not matching the current structure.
// 		_error_page_map["404"];
// 		html_file.open("./html/error404.html", std::ifstream::in);

// 		if (html_file.is_open() == 0)
// 		{
// 			std::cout << REDB << "Error: attempt to open 404 page. File could not be openned" << RESET << std::endl;

// 		}
// 	}

// 	/* read file */

// 	ss << html_file.rdbuf();

// 	/* convert to string */

// 	_html_content = ss.str();

// 	/* get msg body size */
// 	_html_content_size = _html_content.length();

// 	// html file content is
// 	// std::cout << "html File content is <" << _html_content << ">" << std::endl;

// 	/* readbuffer to output stream */
// 	// _html_content << html_file.rdbuf();

// 	html_file.close();
// 	return (0);
// }

// std::string	Response::get_html_content(void) const
// {
// 	return (_html_content);
// }

// size_t	Response::get_html_size(void) const
// {
// 	return (_html_content.length());
// }


// /*
// 	Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
// */
// int		Response::_create_status_line(void)
// {
// 	_status_line = "HTTP/1.1 200 OK\r\n";
// 	return (0);
// }

// std::map<std::string, std::string>	Response::get_error_page_map(void) const
// {
// 	return (_error_page_map);
// }

// std::map<std::string, std::string>	Response::get_response_status_map(void) const
// {
// 	return (_response_status_map);
// }


Response::Response(ConfigParser &configParser, Request &request, char *env[]): _server(configParser), _request(request), _envp(env)
{
	initErrorMap();
	initRespMaps();
	initContentMap();
	// setPath();
	// setBool();
	// if (isAbsoluteURI())
	// 	changeHost();
	// if (_fd.isTimeOut == true)
	// 	_ret = 504;
}


/* ************************************************************************** */

Response::~Response(){}

Response::Cgi::Cgi(){}

Response::Cgi::~Cgi(){}

/* ************************************************************************** */

int Response::isAbsoluteURI()
{
	size_t i;
	if ((i = _path.find("http://"))!= std::string::npos || (i = _path.find("https://")) != std::string::npos)
		return (1);
	return (0);
}

/* ************************************************************************** */

void Response::changeHost()
{
	size_t i;
	size_t j;
	std::string newHost;
	std::string referer = _request.get_referer();
	if ((i = _path.find("http://")) != std::string::npos) {

		j = _path.find("/", i + 7);
		newHost = _path.substr(i + 7, j);
	}

	else if ((i = _path.find("http://")) != std::string::npos) {

		j = _path.find("/", i + 8);
		newHost = _path.substr(i + 8, j);
	}

	if ((i = newHost.find("localhost")) != std::string::npos) {

		newHost.erase(i, 9);
		newHost = "127.0.0.1" + newHost;
	}

	// modified host is not being used and will be delete when function gets out of scope
	_host = newHost; // ADDED this line, is this what we are supposed to do with newHost?
}

/* ************************************************************************** */

void	Response::initErrorMap()
{
	_status_code[100] = "Continue";
	_status_code[200] = "OK";
	_status_code[201] = "Created";
	_status_code[202] = "Accepted";
	_status_code[204] = "No Content";
	_status_code[301] = "Moved Permanently";
	_status_code[302] = "Found";
	_status_code[303] = "See Other";
	_status_code[304] = "Not Modified";
	_status_code[305] = "Use Proxy";
	_status_code[307] = "Temporary Redirect";
	_status_code[400] = "Bad Request";
	_status_code[403] = "Forbidden";
	_status_code[404] = "Not Found";
	_status_code[405] = "Method Not Allowed";
	_status_code[413] = "Payload Too Large";
	_status_code[500] = "Internal Server Error";
	_status_code[504] = "Gateway Timeout";
}


void Response::setBool()
{
	_autoindex = false;
	_cgi   = false;
	_indexcgi = false;
	_l = std::string::npos;
}
/* ************************************************************************** */
void	Response::initRespMaps()
{
	_header["Date"] = "Date: ";
	_header["Connection"] = "Connection: ";
	_header["Content-Type"] = "Content-Type: ";
	_header["Content-Length"] = "Content-Length: ";
	_header["Location"] = "Location: ";
	_header["Allow"] = "Allow: ";
	_header["Cache-Control"] = "Cache-Control: ";
	_header["Content-Disposition"] = "Content-Disposition: ";
}

/* ************************************************************************** */

void	Response::initContentMap()
{
	std::ifstream file;
	std::string line;

	file.open("./src/mime.types");
	if (file.is_open())
	{
		while (getline(file, line))
			_contentType[get_key(line)] = get_value(line);
		file.close();
	}
	else
		std::cout << "Can't open mime.types file" << std::endl;
}

/* ************************************************************************** */

std::string Response::call()
{
	// remove("./tmpFile");
	if (_ret != 200)
		error();
	else {
		_ret = ParsingResponse();
		if (_ret != 200)
			error();
		 else if(_method == "GET")
			getMethod();
		// else if (_method == "POST")
		// 	postMethod();
		// else if (_method == "DELETE")
		// 	deleteMethod();
	}
	headerGen();
	return (_response);
}

/* ************************************************************************** */

void	Response::setPath()
{
	std::string root_path = ROOT; // ADDED Maira
	// if (server.root[server.root.length() - 1] == '/') // ORIGINAL
	// 	server.root.erase(server.root.length() - 1); // ORIGINAL
	
	// this->_path = request.get_path(); // ORIGINAL
	this->_path = _request.get_uri(); // MODIFIED

		
	_path = decodePath();
	_location_index = _request.get_location_index(_serverID);
	
	// _root = ("html/index.html"); // Tats config_parser
	if (_path == "/") // TODO Maira Modified for testing
		_path = root_path + "/index.html"; // TODO Maira Modified for testing
	else // TODO Maira Modified for testing
		_path = root_path + _path; // TODO Maira Modified for testing
	setVersion();
	// _path = _request.get_uri(); // COMMENTED OUT, SET ABOVE
}


std::string Response::decodePath()
{
	std::stringstream output;
	char decoded[3];
	std::string var;
	for (std::string::const_iterator it = _path.begin(); it != _path.end(); it++) {
		if (*it == '%')
		{
			decoded[0] = *(++it);
			decoded[1] = *(++it);
			var  = std::strtol(decoded, NULL, 16);
			output << var;
		}
		else if (*it == '+')
			output << ' ';
		else
			output << *it;
	}
	return (output.str());
}

void	Response::setVersion()
{
	this->_version = "HTTP/1.1 ";
	setQuery();
}

void	Response::setQuery()
{
	this->_query = _request.get_query();
	setRet();
}

// !! Check this out
void	Response::setRet()
{
	this->_ret = _request.get_ret();
	setMethod();
}


void	Response::setMethod()
{
	this->_method = _request.get_method();
	setPort();
}

void	Response::setPort()
{
	this->_port = str2int(_request.get_port());
	setHost();
}

void	Response::setHost()
{
	this->_host = _request.get_hostname();
	setBody();
}

void	Response::setBody()
{
	this->_Reqbody = _request.get_body();
}

std::string Response::get_value(std::string line)
{
	size_t i = line.find(":");
	return (line.substr(0, i));
}

std::string Response::get_key(std::string line)
{
	size_t i = line.find(":");
	return (line.substr(i + 1));
}

std::string	Response::get_index()
{
	return (_index);
}