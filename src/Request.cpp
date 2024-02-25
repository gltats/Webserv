/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 21:23:30 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/25 14:45:18 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Connection.hpp"

Request::Request(std::map<std::string, std::string> &config_map): _request_status(0), _method(""), _uri(""), _protocol_version(""), _user_agent(""), _accept(""), _host(""), _accept_encoding(""), _connection(""), _cache_control(""), _transfer_enconding(""), _body(""), _config_map(config_map)
{
	if (_config_map["allow_GET"].compare("y") == 0)
		_allow_GET = true;
	else
		_allow_GET = false;

	if (_config_map["allow_POST"].compare("y") == 0)
		_allow_POST = true;
	else
		_allow_POST = false;

	if (_config_map["allow_DELETE"].compare("y") == 0)
		_allow_DELETE = true;
	else
		_allow_DELETE = false;

	std::cout << "Request default constructor " << std::endl;

}

Request			&Request::operator=(Request const &rhs)
{
	(void)rhs;
	return (*this);
}

// Request::Request(Request const &src)
// {
// 	(void)src;
// }

Request::~Request(void)
{
	std::cout << "Request deconstructor" << std::endl;
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
	if (buffer.substr(i, i + 4).compare("GET ") == 0 && _allow_GET == true)
	{
		_method = "GET";
		i = i + 4;
	}
	else if (buffer.substr(i, i + 5).compare("POST ") == 0 && _allow_POST == true)
	{
		_method = "POST";
		i = i + 5;
	}
	else if (buffer.substr(i, i + 7).compare("DELETE ") == 0 && _allow_DELETE == true)
	{
		_method = "DELETE";
		i = i + 7;
	}
	else
	{
		std::cout << REDB << "Method: " << buffer.substr(i, i + 4) << RESET << std::endl;
		throw MethodNotSupported();
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
		else
			throw InvalidRequest();
	}

	return (0);
}

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

int Request::_parser_request_header(std::string buffer)
{
	_user_agent = _get_key_value(buffer, "User-Agent: ");
	_accept = _get_key_value(buffer, "Accept: ");
	_host = _get_key_value(buffer, "Host: ");
	_connection = _get_key_value(buffer, "Connection: ");
	_accept_encoding = _get_key_value(buffer, "Accept-Encoding: ");
	_cache_control = _get_key_value(buffer, "Cache-Control: ");
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

	// Transfer Encoding
	_transfer_enconding = _get_key_value(buffer, "Transfer-Encoding: ");

	return (0);
}

int Request::_parser_body(std::string buffer)
{
	// get index of end of the header = empty line between headers and body
	_index_end_of_headers = buffer.find("\r\n\r\n");
	_index_start_of_body = _index_end_of_headers + 4;

	std::cout << CYAN << "_index_end_of_headers is " << _index_end_of_headers << " - char is " << buffer[_index_end_of_headers] << RESET <<  std::endl; //remove
	if (_index_end_of_headers == -1)
	{
		print_error("Request does not contain mandatory HTTP1.1 empty line.");
		throw InvalidRequest();
	}
	else if (_index_start_of_body < (int)buffer.length())
	{
		std::cout << CYAN << "_body first " << buffer[_index_start_of_body] << " body second " << buffer[std::string::npos - 1] <<  std::endl; //remove

		// threre is a body
		_body = buffer.substr(_index_start_of_body, std::string::npos - 1);
	}
	else
	{
		std::cout << CYAN << "No body message " <<  std::endl; //remove
		_body = "";
	}

	// std::cout << "->Transfer enconding is "  << get_transfer_enconding() << std::endl; // remove
	// if (_transfer_enconding.compare("chunked") == 0)
	// {
	// 	std::cout << "CHUNKED" << std::endl; // remove
	// 	_handle_chunked();
	// }
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
	std::cout << std::endl;
	std::cout << "\tBody:\n<" << _body << ">" << std::endl;
	
	std::cout << std::endl;
	std::cout << RESET;
}

int Request::get_request_status(void) const
{
	return (_request_status);
}

void	Request::read_request(char const *request_buffer)
{
	// convert char buffer to string
	std::string str(request_buffer); // string constructor that takes a null terminated string
	
	// clean up
	_request_status = 0;
	_method.clear();
	_uri.clear();
	_protocol_version.clear();
	_user_agent.clear();
	_accept.clear();
	_host.clear();
	_accept_encoding.clear();
	_connection.clear();
	_cache_control.clear();
	_transfer_enconding.clear();
	_body.clear();


	// parser
	_parse_request_line(str);
	_parser_general_header(str);
	_parser_request_header(str);
	_parser_body(str);
}
std::string		Request::get_uri(void) const
{
	return (_uri);
}

std::string		Request::get_method(void) const
{
	return (_method);
}

std::string		Request::get_protocol_version(void) const
{
	return (_protocol_version);
}

std::string		Request::get_user_agent(void) const
{
	return (_user_agent);
}

std::string		Request::get_host(void) const
{
	return (_host);
}

std::string		Request::get_accept_encoding(void) const
{
	return (_accept_encoding);
}

std::string		Request::get_connection(void) const
{
	return (_connection);
}

std::string		Request::get_cache_control(void) const
{
	return (_cache_control);
}

std::string		Request::get_body(void) const
{
	return (_body);
}


std::string		Request::get_transfer_enconding(void) const
{
	return (_transfer_enconding);
}


bool		Request::get_allow_GET(void) const
{
	return (_allow_GET);
}

bool		Request::get_allow_POST(void) const
{		
	return (_allow_POST);
}

bool		Request::get_allow_DELETE(void) const
{
	return (_allow_DELETE);
}

/*
Transfer-Encoding: chunked


chunked
Data is sent in a series of chunks. 
The Content-Length header is omitted in this case
and at the beginning of each chunk you need to add
the length of the current chunk in hexadecimal format,
followed by '\r\n' and then the chunk itself, followed by another '\r\n'.
The terminating chunk is a regular chunk, with the exception that its length is zero.
It is followed by the trailer, which consists of a (possibly empty) sequence of header fields.

Transfer-Encoding: gzip, chunked
This is why Transfer-Encoding is defined as overriding Content-Length,

A server MAY reject a request that contains both Content-Length and Transfer-Encoding or process such a request in accordance with the Transfer-Encoding alone. Regardless, the server MUST close the connection after responding to such a request to avoid the potential attacks.

*/

// if (_transfer_enconding.compare("chunked"))
// {
// 	_is_chunked = true;

// }
/*
POST /some/path HTTP/1.1
Host: example.com
Content-Type: application/json
Transfer-Encoding: chunked

7\r\n
abcdefg\r\n
6\r\n
123456\r\n
0\r\n
\r\n
*/

void		Request::_handle_chunked(void)
{
	size_t			total_content_length = 0;
	int				chunk_length = 0;
	size_t 			pos_start = 0;
	size_t 			pos_end = 0;
	std::string 	chunked_str = "";

	while (1)
	{
		// get length of current chunk
		pos_end = _body.find("\r\n", pos_start);
		
		std::cout << "pos_end is " << pos_end << ", pos_start is " << pos_start << std::endl;
		chunk_length = str2int(_body.substr(pos_start, pos_end - 1));
		if (pos_end == std::string::npos || chunk_length == -1)
		{
			print_error("_handle_chunked");
			throw InvalidRequest();
		}
		// get chunk
		pos_start = pos_end + 2;
		pos_end = _body.find("\r\n", pos_start);
		chunked_str.append(_body.substr(pos_start, pos_end));
		if (pos_end - pos_start != (size_t) chunk_length)
		{
			print_error("_handle_chunked: Chunk real size does not match passed chunk size");
			std::cout << "Lenght Chunk: Read from chunk is" << chunk_length << ", calculated from message " << pos_end - pos_start << std::endl; //remove
			throw InvalidRequest();
		}
		total_content_length += (size_t) chunk_length;


		std::cout << "chunk len: <" << chunk_length << ">" << std::endl;
		std::cout << "chunk: <" << chunked_str << ">" << std::endl;
		std::cout << "total_content_length: <" << total_content_length << ">" << std::endl;

		// go forward for next loop
		pos_start = pos_end + 2;
	}
	


}