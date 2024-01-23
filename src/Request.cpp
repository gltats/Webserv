#include "Request.hpp"

Request::Request(void):  _request_status(0), _method(""), _uri(""), _protocol_version(""), _user_agent(""), _accept(""), _host(""), _accept_encoding(""), _connection(""), _cache_control("") //, _content_lenght(0)
{

}

Request			&Request::operator=(Request const &rhs)
{
	(void)rhs;
	return (*this);
}

Request::Request(Request const &src)
{
	(void)src;
}

Request::~Request(void)
{

}

/*
	Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
*/
int	Request::_parse_request_line(std::string buffer)
{
	int i = 0;

	// ignore initial CRLF
	while(buffer[i] != '\0')
	{
		if (buffer[i] == CR && buffer[i + 1] == LF)
			i++;
		else if (buffer[i] == 'G' || buffer[i] == 'P' || buffer[i] == 'D')
			break;
		else
		{
			std::cout << REDB << "Error: _parse_request_line. " \
			<< "Request Line in incorrect format" << RESET << std::endl;
			return(-1);
		}

	}
	// Get Method : allowed GET, POST, DELETE
	if (buffer.substr(i, i + 4).compare("GET ") == 0)
	{
		_method = "GET";
		i = i + 4;
	}
	else if (buffer.substr(i, i + 5).compare("POST ") == 0)
	{
		_method = "POST";
		i = i + 5;
	}
	else if (buffer.substr(i, i + 7).compare("DELETE ") == 0)
	{
		_method = "DELETE";
		i = i + 7;
	}
	else
	{
		std::cout << REDB << "Error: _parse_request_line. " \
		<< "Request Line incorrect method. Only HTTP is accepted" << RESET << std::endl;
		return(-1);
	}

	// Get URI len
	int j = i;
	while (buffer[j] != '\0')
	{
		if (buffer[j] == SP)
			break;
		j++;
	}

	//Get URI string
	if (buffer[j] == '\0')
	{
		std::cout << REDB << "Error: _parse_request_line. " \
		<< "Request Line URI format" << RESET << std::endl;
		return(-1);
	}
	else
	{
		_uri = buffer.substr(i, j - i);
		if (_uri.compare("/") == 0)
		{
			std::cout << REDB << "Error: _parse_request_line. " \
			<< "Request Line No URI passed." << RESET << std::endl;
			return(-1);
		}
		_uri = buffer.substr(i + 1, j - i - 1);
	}


	// Check Protocol Version HTTP/1.1
	int k = j;
	while (buffer[k] != '\0' && buffer[k + 1] != '\0')
	{
		if (buffer[k] == CR && buffer[k + 1] == LF)
			break;
		k++;
	}

	if (buffer[k] == '\0' || buffer[k + 1] == '\0')
	{
		std::cout << REDB << "Error: _parse_request_line. " \
		<< "Request Line Protocol Version format" << RESET << std::endl;
		return(-1);
	}
	else
	{
		if (buffer.substr(j + 1, k).compare("HTTP/1.1"))
			_protocol_version = "HTTP/1.1";
	}

	return (0);
}

/*
 	request-header =	  Accept                   ; Section 14.1
						| Accept-Charset           ; Section 14.2
						| Accept-Encoding          ; Section 14.3
						| Accept-Language          ; Section 14.4
						| Authorization            ; Section 14.8
						| Expect                   ; Section 14.20
						| From                     ; Section 14.22
						| Host                     ; Section 14.23
						| If-Match                 ; Section 14.24
						| If-Modified-Since        ; Section 14.25
						| If-None-Match            ; Section 14.26
						| If-Range                 ; Section 14.27
						| If-Unmodified-Since      ; Section 14.28
						| Max-Forwards             ; Section 14.31
						| Proxy-Authorization      ; Section 14.34
						| Range                    ; Section 14.35
						| Referer                  ; Section 14.36
						| TE                       ; Section 14.39
						| User-Agent               ; Section 14.43
*/

std::string Request::_get_key_value(std::string buffer, std::string needle)
{
	int start = 0;
	int end = 0;
	int i;

	start = buffer.find(needle);
	if (start != -1)
	{
		i = start;
		while (buffer[i] != '\0')
		{
			if (buffer[i] == CR && buffer[i + 1] == LF)
			{
				end = i - 1;
				break;
			}
			i++;
		}
		return( buffer.substr(start + needle.length(), end - (start + needle.length()) + 1) );
	}
	return ("");
}

int Request::_parser_request_header(std::string buffer)
{
	_user_agent = _get_key_value(buffer, "User-Agent: ");
	_accept = _get_key_value(buffer, "Accept: ");
	_host = _get_key_value(buffer, "Host: ");

	return (0);
}

/*
		general-header =  Cache-Control            ; Section 14.9
					| Connection               ; Section 14.10
					| Date                     ; Section 14.18
					| Pragma                   ; Section 14.32
					| Trailer                  ; Section 14.40
					| Transfer-Encoding        ; Section 14.41
					| Upgrade                  ; Section 14.42
					| Via                      ; Section 14.45
					| Warning                  ; Section 14.46
*/
int Request::_parser_general_header(std::string buffer)
{
	// Cache Control
	_cache_control = _get_key_value(buffer, "Cache-Control: ");

	// Connection
	_connection = _get_key_value(buffer, "Connection: ");

	return (0);
}

void Request::print_request(void) const
{
	std::cout << YELLOW;
	std::cout << "\tRequest Line" << std::endl;
	std::cout << "\t\tMethod:<" << _method << ">" << std::endl;
	std::cout << "\t\tUri:<" << _uri << ">" << std::endl;
	std::cout << "\t\tProtocol/Version:<" << _protocol_version << ">" << std::endl;
	std::cout << std::endl;

	std::cout << "\tRequest Header" << std::endl;
	std::cout << "\t\tUser-Agent:<" << _user_agent << ">" << std::endl;
	std::cout << "\t\tAccept:<" << _accept << ">" << std::endl;
	std::cout << "\t\tCache-Control:<" << _cache_control << ">" << std::endl;
	std::cout << "\t\tHost:<" << _host << ">" << std::endl;
	std::cout << "\t\tAccept-Encoding:<" << _accept_encoding << ">" << std::endl;

	std::cout << std::endl;
	std::cout << "\tGeneral Header" << std::endl;
	std::cout << "\t\tConnection:<" << _connection << ">" << std::endl;
	std::cout << RESET;
}

int Request::get_request_status(void) const
{
	return (_request_status);
}

void	Request::read_request(char const *request_buffer)
{
	std::string str(request_buffer); // string constructor that takes a null terminated string

	_parse_request_line(str);
	_parser_general_header(str);
	_parser_request_header(str);

}
std::string Request::get_uri(void) const
{
	return (_uri);
}
