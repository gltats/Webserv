

#include <iostream>
#include <map>
#include "includes/color_code.hpp"

// value to trigger "Request-URI Too Long" status 414
#define MAX_URI_LEN 8000 

bool        _allow_GET = true;
bool        _allow_POST = true;
bool        _allow_DELETE = true;

std::string _buffer;

std::string _body;
std::string _headers;

std::string _method;
std::string _uri;
std::string _protocol;
std::string _version;

std::unordered_map<std::string, std::string> _headers_map;

class MethodNotAllowedException: public std::exception
{
	public:
		virtual const char* what() const throw()
        {
            return ("Exception: Status 405, Method not supported. Webserv supports GET, POST and DELETE. \nEnable them in Config File."); 
        }
};

class BadRequestException: public std::exception
{
	public:
		virtual const char* what() const throw()
        {
           return ("Exception: Status 400, Bad Request"); 
        }
};

class HTTPVersionNotSupportedException: public std::exception
{
	public:
		virtual const char* what() const throw()
        {
            return ("Exception: Status 505, Version not supported. Webserv supports HTTP/1.1"); 
        }
};

class RequestURITooLongException: public std::exception
{
	public:
		virtual const char* what() const throw()
        {
            return ("Exception: Status 414, Request-URI Too Long"); 
        }
};





// it returns the string it finds from begin until delimiter (delimiter exclusive)
// this function destroys str at each execution removing characters from begin until delimiters end
std::string           extract_until_delimiter(std::string *str, std::string delimiter)
{
    size_t      idx;
    std::string extract = "";

    idx = str->find(delimiter, 0);
    if (idx != std::string::npos)
    {
        extract = str->substr(0, idx);
        str->erase(0, idx + delimiter.length());
    }
    else if (str->length() > 0)
    {
        extract = *str;
        str->clear();
    }
    return (extract);
}

void    split_headers_body(void)
{
    std::string copy_buffer = _buffer;
    _headers = extract_until_delimiter(&copy_buffer, "\r\n\r\n");
    _body = copy_buffer;
}

void    parse_request_line(std::string request_line)
{
    _method = extract_until_delimiter(&request_line, " ");
    _uri = extract_until_delimiter(&request_line, " ");   
    _protocol = extract_until_delimiter(&request_line, "/");  
    _version = request_line;
    request_line.clear();
}

void    check_method(void)
{
    if ((_method.compare("GET") == 0 && _allow_GET == true)
        || (_method.compare("POST") == 0 && _allow_POST == true)
        || (_method.compare("DELETE") == 0 && _allow_DELETE == true))
    {
        std::cout << CYAN << "Valid Method : " << _method << RESET << std::endl;
    }
    else
    {
         std::cerr << REDB << "Method :" << _method << RESET << std::endl;
        throw MethodNotAllowedException();
    }

}

void    check_protocol(void)
{
    if (_protocol.compare("HTTP") == 0)
    {
        std::cout << CYAN << "Valid Protocol : " << _protocol << RESET << std::endl;
    }
    else
    {
        std::cerr << REDB << "Invalid Protocol :" << _protocol << RESET << std::endl;
        throw BadRequestException();
    }
}

void    check_version(void)
{
    if (_version.compare("1.1") == 0)
    {
        std::cout << CYAN << "Valid Version : " << _version << RESET << std::endl;
    }
    else
    {
        std::cerr << REDB << "Version :" << _version << RESET << std::endl;
        throw HTTPVersionNotSupportedException();
    }

}

void    check_uri(void)
{
    size_t len = _uri.length();
    if (len == 0)
    {
        std::cerr << REDB << "Empty uri" << RESET << std::endl;
        throw BadRequestException();
    }
    else if (len > MAX_URI_LEN )
    {
        std::cerr << REDB << "Uri too long" << RESET << std::endl;
        throw RequestURITooLongException();
    }
    std::cout << CYAN << "URI Version : " << _uri << RESET << std::endl;
}

/*
    we could have getters for every entry
    or a map with what was passed, and what you want to use you try to get it from a map
*/
void    parser_header_line(std::string line)
{
    // get key
    std::string key = extract_until_delimiter(&line, ": ");

    // get value
    std::string value = line;

    // match
    _headers_map[key] = value;
}

void    check_headers_map(void)
{
     // create an map iterator element
	std::unordered_map<std::string, std::string>::iterator it;

	// clean up any still open file descriptors or Connections
	for (it = _headers_map.begin(); it != _headers_map.end(); it++)
	{
        if (it->first.length() == 0 || it->second.length() == 0)
        {
            std::cerr << REDB << "Key or Value empty" << RESET << std::endl;
            throw BadRequestException();
        }
    } 
}


void    print_headers_map(void)
{
     // create an map iterator element
	std::unordered_map<std::string, std::string>::iterator it;

	// clean up any still open file descriptors or Connections
	for (it = _headers_map.begin(); it != _headers_map.end(); it++)
	{
        std::cout << "Key:<" << it->first << "> | Value:<" << it->second << ">" << std::endl;
    } 
}

std::string    get_header_per_key(std::string header_key)
{
    std::unordered_map<std::string, std::string>::iterator it = _headers_map.find(header_key);

    if (it == _headers_map.end())
    {
        return ("");
    }
    return(it->second);
}

void    response(void)
{
    std::cout <<  "initial string: <" << _buffer << ">" << std::endl;
    std::cout << std::endl;
    
    split_headers_body();

    std::string line;
    // parse first line -> request line
    line = extract_until_delimiter(&_headers, "\r\n");
    parse_request_line(line);

    while (1)
    {
        line.clear();
        line = extract_until_delimiter(&_headers, "\r\n");
        if (line.length() == 0)
            break; 
        parser_header_line(line);
    }


    try
    {
        check_method();
        check_uri();
        check_protocol();
        check_version();
        check_headers_map();
    }
    catch(const MethodNotAllowedException& e)
    {
        std::cerr << REDB << e.what() << RESET << std::endl;
        exit (2);
    }
    catch(const BadRequestException& e)
    {
        std::cerr << REDB << e.what() << RESET << std::endl;
        exit (2);
    }
    catch(const HTTPVersionNotSupportedException& e)
    {
        std::cerr << REDB << e.what() << RESET << std::endl;
        exit (2);
    }
    catch(const RequestURITooLongException& e)
    {
        std::cerr << REDB << e.what() << RESET << std::endl;
        exit (2);
    }
    

    std::cout << std::endl;
    std::cout << "PARSED :" << std::endl;
    std::cout << "method   <" << _method << ">" << std::endl;
    std::cout << "uri      <" << _uri << ">" << std::endl;
    std::cout << "protocol <" << _protocol << ">" << std::endl;
    std::cout << "version  <" << _version << ">" << std::endl;

    std::cout << "-------- Headers --------" << std::endl;
    print_headers_map();

    std::cout << "---------GET HEADER VALUE-------" << std::endl;
    std::cout << "Existing Key 'Host' value is : <" << get_header_per_key("Host") << ">" << std::endl;
    std::cout << "Non-Existing Key 'Lasagna' value is : <" << get_header_per_key("Lasagna") << ">" << std::endl;

}


/*
    wrong behavior if a add an additional space : finds GET but not the rest
     _buffer = "GET  /html/basic.html HTTP/1.1\r\nHost: Maira\r\nContent-Lenght: 12\r\n\r\nMozilla i am here\nTransfer-Encoding: chunked\nDeveloper Network";
    result:
    PARSED :
        method   <GET>
        uri      <>
        protocol <>
        version  <html/basic.html HTTP/1.1>
*/

int     main(void)
{

    _buffer = "GET /html/basic.html HTTP/1.1\r\nHost: Maira\r\nContent-Lenght: 12\r\nTransfer-Encoding: chunked\r\nAccept: enconding\r\n\r\nMozilla i am here\nSomething written\nDeveloper Network";

    response();
    return (0);
}