/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 20:48:56 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/19 21:10:58 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Connection.hpp"

Request::Request(Connection &connection): _connection(connection)
{
    _cleanMemory();

    // for testing: must come from configParser
    _allow_GET = true; // get from configParser
    _allow_POST = true; // get from configParser
    _allow_DELETE = true; // get from configParser

}

Request::~Request(void)
{

}


void    Request::_cleanMemory(void)
{
    _body.clear();
    _method.clear();
    _headers.clear();
    _method.clear();
    _uri.clear();
    _protocol.clear();
    _version.clear();

    _content_len = 0;
    _error = 0;

    _headers_map.clear();
}

void                Request::parse_request(char const *buffer)
{
    std::string     line;
    std::string     request_buffer = buffer;

    _cleanMemory();

    try
    {
        _split_headers_body(request_buffer);

        // parse first line -> request line
        line = _extract_until_delimiter(&_headers, "\r\n");
        _process_request_line(line);

        while (1)
        {
            line.clear();
            line = _extract_until_delimiter(&_headers, "\r\n");
            if (line.length() == 0)
                break;
            _process_header_line(line);
        }

        // modify case-insensitive header values
        _modify_header_values_tolower();
 
		// check if mandatory fields where sent in the request
		_check_mandatory_header_fields();

		// identify server and set the server_id which matches the configParser server blocks
		_identify_server();

		// check if port and hostname in request matches written socket
		_check_valid_port();
		// _check_valid_hostname(); // not possible as input is always localhost

		// std::cout << CYAN << "_server_id " << _server_id << ", idenfied server is fd = " <<  _connection.get_server_socket() << ", port =  " << _connection.get_configParser().get_listen(_server_id) << std::endl;
		std::cout << CYAN << "_server_id " << _server_id << ", idenfied server is fd = " <<  _connection.get_server_socket() << ", port =  " << get_port() << ", server_name = " << get_hostname() << RESET << std::endl; // remove

		// based on server id, check allowed methods
		// _check_allowed_method(); // TODO uncomment
 
        _process_body(_body);
    }
    catch(const MethodNotAllowedException& e)
    {
        _error = 405;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }
    catch(const BadRequestException& e)
    {
        _error = 400;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }
    catch(const HTTPVersionNotSupportedException& e)
    {
        _error = 505;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }
    catch(const UnsupportedMediaTypeException& e)
    {
        _error = 415;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }
    catch(const RequestEntityTooLargeException& e)
    {
        _error = 413;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }
    catch(const RequestURITooLongException& e)
    {
        _error = 414;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }
    catch(const LengthRequiredException &e)
    {
         _error = 411;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }
    catch(const NotImplemented& e)
    {
        _error = 501;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }
}

void	Request::_split_host_in_hostname_port(void)
{
	std::string	host = get_host();
	std::string	hostname;
	std::string	port;
	size_t		index;

	if (host.length() > 0)
	{
		index = host.find(":");
		if (index != std::string::npos)
		{
			hostname = host.substr(0, index);
			if (index + 1 < host.length())
			{
				port = host.substr(index + 1);
				_headers_map["hostname"] = hostname;
				_headers_map["port"] = port;
			}
			else
			{
				print_error("Port not found in Host");
				throw BadRequestException();
			}
		}
		else
		{
			print_error("Error in Host  <:> not found in Host");
			throw BadRequestException();
		}
		// otherwise a port was not sent
	}
}


/*
in an Nginx configuration with multiple server blocks listening on the same port,
Nginx adopts a fallback mechanism to determine which server block to use for requests
that do not match any of the specified server_name directives.
Here's how Nginx decides on the fallback:

First Server Block Selection: In the absence of a defined default_server flag,
Nginx automatically uses the first server block (in the order they
appear in the configuration file) that listens on the matching port as
the default server for that port. This means that if a request comes in with a
Host header that doesn't match any of the server_name values across all
server blocks listening on the same port, Nginx will send the request to the
first server block encountered in the configuration that is listening on the requested port.
*/
void    Request::_identify_server(void)
{
    int             nb_of_servers = _connection.get_configParser().get_nb_of_servers();
    int             socket = _connection.get_server_socket();
    int             *servers_fd = _connection.get_servers_fd();

    int             first_match = -1; // first match to file descriptor
	int				default_flag = -1;
    bool            once = false;


	_split_host_in_hostname_port();

    // count amount of servers using the same port/socket file descriptor
    for (int i = 0; i < nb_of_servers; i++)
    {
        if (socket == servers_fd[i]) // should not look for server_fd ERRADO
        {
			if (_connection.get_configParser().get_server_name(i).compare(get_hostname()) == 0)
            {
                _server_id = i;
                return ;
            }

			// save the server_id with a default flag, if any
			if (_connection.get_configParser().get_default_server(i).compare("y") == 0)
			{
				default_flag = i; // save server id which has a default_server flag
				once = true; // fallback not necessary as a server is defined as default
			}

			// get first server with the port in case of fallback necessary
            if (once == false)
            {
                first_match = i;
                once = true;
            }
        }
    }

	// search for default_server flag

    if (default_flag > 0)
    {
        // a default server was desfined
        _server_id = default_flag;
		return ;

    }
    else if (once == true)
    {
		// Fallback
        // no server name from server block(configuration file) match received Host
        // use default server
        _server_id = first_match;
		return ;
    }
	std::cout << REDB << "No server identified" << std::endl;
	throw BadRequestException();  // is this the best exception for that? should it be server error?

}

void	Request::_check_valid_port(void)
{
	// check port matches the socket port
	if (_connection.get_configParser().get_listen(_server_id).compare(get_port()) != 0)
	{
		print_error("Error in Port in request does not match socket port");
		std::cout << REDB << "Socket port '" << _connection.get_configParser().get_listen(_server_id) << "', server_id" << _server_id << std::endl;
		std::cout << REDB << "Request Host port '" << get_port() <<  "'" << std::endl;
		throw BadRequestException();
	}
    
}

void	Request::_check_valid_hostname(void)
{
	// check port matches the socket hostname
	if (_connection.get_configParser().get_server_name(_server_id).compare(get_hostname()) != 0)
	{
		print_error("Error in Hostname in request does not match server name");
		std::cout << REDB << "Server hostname'" << _connection.get_configParser().get_server_name(_server_id) << "', server_id" << _server_id << std::endl;
		std::cout << REDB << "Request hostname '" << get_hostname() <<  "'" << std::endl;
		throw BadRequestException();
	}
}



// General Functions

/*
    Convert string to lower case. Used for Case insensitive string
    Each header field consists of a case-insensitive field name followed
    by a colon (":"),
*/
std::string Request::_convert_tolower(std::string const &str) const
{
    std::string out = str;
    for (size_t i = 0; i < str.length(); i++)
    {
        out[i] = tolower(str[i]);
    }
    return (out);
}

/*
    RFC7230
    A recipient MUST parse for such bad whitespace and remove
    it before interpreting the protocol element.

        OWS            = *( SP / HTAB )
                    ; optional whitespace
        RWS            = 1*( SP / HTAB )
                    ; required whitespace
        BWS            = OWS
                    ; "bad" whitespace

    HTAB (horizontal tab)
    SP (space)
*/
void                Request::_remove_leading_whitespace(std::string &str)
{
    size_t  idx;

    while (1)
    {
        // find SP or HTAB
        idx = str.find_first_of(" \t", 0);
        if (idx == std::string::npos || idx != 0)
        {
            // if there is none, return or is not at the index 0
            return ;
        }
        // remove leading whitespace
        str.erase(0, 1);
    }
}
void                Request::_remove_trailing_whitespace(std::string &str)
{
    size_t  idx;

    while (1)
    {
        idx = str.find_first_of(" \t", str.length() - 1);
        if (idx == std::string::npos || idx != str.length() - 1)
        {
            // if there is none, return or is not at the index last
                return ;
        }
        // remove trailing LWS
        str.erase(str.length() - 1, 1);
    }
}

// it returns the string it finds from begin until delimiter (delimiter exclusive)
// this function destroys str at each execution removing characters from begin until delimiters end
std::string         Request::_extract_until_delimiter(std::string *str, std::string delimiter)
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


/*
    RFC 7230
    Historically(RFC2616), HTTP header field values could be extended over
    multiple lines by preceding each extra line with at least one space
    or horizontal tab (obs-fold).  This specification deprecates such
    line folding except within the message/http media type
    (Section 8.3.1).  A sender MUST NOT generate a message that includes
    line folding (i.e., that has any field-value that contains a match to
    the obs-fold rule) unless the message is intended for packaging
    within the message/http media type.

    A server that receives an obs-fold in a request message that is not
    within a message/http container MUST either reject the message by
    sending a 400 (Bad Request), preferably with a representation
    explaining that obsolete line folding is unacceptable, or replace
    each received obs-fold with one or more SP octets prior to
    interpreting the field value or forwarding the message downstream.

*/
void                Request::_check_field_value_obs_fold(std::string field_value)
{
   if (field_value[0] == ' ' || field_value[0] == '\t')
   {
        std::cerr << REDB << "whitespace in header field name, or deprecated field value \
        fold or unsupported Content-Type message/http" << RESET << std::endl;
        throw UnsupportedMediaTypeException();
   }
}
void                Request::_split_headers_body(std::string &buffer)
{
    if (buffer.length() == 0)
    {
        std::cerr << REDB << "Empty request" << RESET << std::endl;
        throw BadRequestException();
    }
    else if (buffer.find( "\r\n\r\n") == std::string::npos)
    {
        std::cerr << REDB << "Empty line indicating the end of the header section not found" << RESET << std::endl;
        throw BadRequestException();
    }
    std::string copy_buffer = buffer;
    _headers = _extract_until_delimiter(&copy_buffer, "\r\n\r\n");
    _body = copy_buffer;
}

size_t              Request::_convert_str2size_t(std::string str)
{
    char*           endptr;
    size_t          nb;

    if (str.length() == 0)
        nb = 0;
    else if (str.length() > 0 && str[0] == '0')
        nb = 0;
    else
    {
        nb = strtol(str.c_str(), &endptr, 10);
        if (nb == 0 || strlen(endptr) > 0)
        {
            std::cerr << REDB << "error to convert string " << str << " to size_t" << RESET << std::endl;
            throw BadRequestException();
        }
    }
    return (nb);
}
size_t              Request::_convert_str2hex(std::string str)
{
    char*           endptr;
    size_t          nb;

    if (str.length() == 0)
        nb = 0;
    else if (str.length() > 0 && str[0] == '0')
        nb = 0;
    else
    {
        nb = strtol(str.c_str(), &endptr, 16);
        if (nb == 0 || strlen(endptr) > 0)
        {
            std::cerr << REDB << "error to convert string " << str << " to hex" << RESET << std::endl;
            throw BadRequestException();
        }
    }
    return (nb);
}


// ---------------- Request Start Line -----------------//

/*
    RFC2616
    In the interest of robustness, servers SHOULD ignore any empty
   line(s) received where a Request-Line is expected. In other words, if
   the server is reading the protocol stream at the beginning of a
   message and receives a CRLF first, it should ignore the CRLF.

*/
/*
    The Request-Line begins with a method token, followed by the
   Request-URI and the protocol version, and ending with CRLF. The
   elements are separated by SP characters. No CR or LF is allowed
   except in the final CRLF sequence.

        Request-Line   = Method SP Request-URI SP HTTP-Version CRLF


    RFC7230
   Recipients typically parse the request-line into its component parts
   by splitting on whitespace (see Section 3.5), since no whitespace is
   allowed in the three components.  Unfortunately, some user agents
   fail to properly encode or exclude whitespace found in hypertext
   references, resulting in those disallowed characters being sent in a
   request-target.

   Recipients of an invalid request-line SHOULD respond with either a
   400 (Bad Request) error or a 301 (Moved Permanently) redirect with
   the request-target properly encoded.  A recipient SHOULD NOT attempt
   to autocorrect and then process the request without a redirect, since
   the invalid request-line might be deliberately crafted to bypass
   security filters along the request chain.

    URI is case-insensitive
    The scheme and host are case-insensitive and normally provided in lowercase; all other
    components are compared in a case-sensitive manner.  Characters other
    than those in the "reserved" set are equivalent to their
    percent-encoded octets: the normal form is to not encode them (see
    Sections 2.1 and 2.2 of [RFC3986]).

    For example, the following three URIs are equivalent:

    http://example.com:80/~smith/home.html
    http://EXAMPLE.com/%7Esmith/home.html
    http://EXAMPLE.com:/%7esmith/home.html

*/


/*
    RFC7230
    Various ad hoc limitations on request-line length are found in
    practice.  It is RECOMMENDED that all HTTP senders and recipients
    support, at a minimum, request-line lengths of 8000 octets.
*/
void                Request::_check_request_line_size(std::string str)
{
    size_t len = str.length();
    if (len == 0)
    {
        std::cerr << REDB << "No request line found" << RESET << std::endl;
        throw BadRequestException();
    }
    else if (len > MAX_REQUEST_LINE_LEN)
    {
        std::cerr << REDB << "Uri too long" << RESET << std::endl;
        throw RequestURITooLongException();
    }
}
void                Request::_parse_request_line(std::string request_line)
{
    _method = _extract_until_delimiter(&request_line, " ");
    _uri = _convert_tolower(_extract_until_delimiter(&request_line, " "));
    _protocol = _extract_until_delimiter(&request_line, "/");
    _version = request_line;
    request_line.clear();
}


/*
    RFC2616
    An origin server SHOULD return the status code 405 (Method Not Allowed)
    if the method is known by the origin server but not allowed for the
    requested resource, and 501 (Not Implemented) if the method is
    unrecognized or not implemented by the origin server.

    RFC7230
    Recipients of an invalid request-line SHOULD respond with either a
    400 (Bad Request) error or a 301 (Moved Permanently) redirect with
    the request-target properly encoded.  A recipient SHOULD NOT attempt
    to autocorrect and then process the request without a redirect, since
    the invalid request-line might be deliberately crafted to bypass
    security filters along the request chain.

    possible methods in HTTP 1.1:
    GET: Retrieves data from the server.
    HEAD: Retrieves only the headers of a resource without the body content.
    POST: Submits data to be processed to a specified resource.
    PUT: Uploads a representation of the specified resource.
    DELETE: Deletes the specified resource.
    OPTIONS: Retrieves the HTTP methods supported by the server for a specified URL.
    TRACE: Echoes the received request so that a client can see what (if any) changes or additions have been made by intermediate servers.
    CONNECT: Converts the request connection to a transparent TCP/IP tunnel.
    PATCH: Applies partial modifications to a resource.

*/
void               Request::_check_method_syntax(void)
{
 	if (_method.length() == 0)
    {
        std::cerr << REDB << "Method is empty" << RESET << std::endl;
        throw BadRequestException();
    }
    else if (_method.compare("GET") != 0 && _method.compare("POST") != 0 && _method.compare("DELETE") != 0)
    {
        if (_method.compare("HEAD") == 0 || _method.compare("PUT") == 0 || _method.compare("OPTIONS") == 0
        || _method.compare("TRACE") == 0 || _method.compare("CONNECT") == 0  || _method.compare("PATCH") == 0 )
        {
            std::cerr << REDB << "Method <" << _method << ">" << RESET << std::endl;
            throw  NotImplemented();
        }
        else
        {
            std::cerr << REDB << "Method <" << _method << ">" << RESET << std::endl;
            throw BadRequestException();
        }
    }
    // Valid Method
}

void			Request::_check_allowed_method(void)
{
	if ((_method.compare("GET") == 0 && _allow_GET == false)
        || (_method.compare("POST") == 0 && _allow_POST == false)
        || (_method.compare("DELETE") == 0 && _allow_DELETE == false))
    {
        std::cerr << REDB << "Method <" << _method << ">" << RESET << std::endl;
        throw MethodNotAllowedException();
    }
}

void                Request::_check_protocol(void)
{
    if (_protocol.length() == 4 && _protocol.compare("HTTP") == 0)
    {
        std::cout << CYAN << "Valid Protocol <" << _protocol << ">" << RESET << std::endl;
    }
    else
    {
        std::cerr << REDB << "Invalid Protocol <" << _protocol << ">" << RESET << std::endl;
        throw BadRequestException();
    }
}
void                Request::_check_version(void)
{
    if (_version.length() == 3 && _version.compare("1.1") == 0)
    {
        std::cout << CYAN << "Valid Version <" << _version << ">" <<  RESET << std::endl;
    }
    else
    {
        std::cerr << REDB << "Version <" << _version << ">" << RESET << std::endl;
        throw HTTPVersionNotSupportedException();
    }
}


/*
    Recipients of an invalid request-line SHOULD respond with either a
   400 (Bad Request) error or a 301 (Moved Permanently) redirect with
   the request-target properly encoded.  A recipient SHOULD NOT attempt
   to autocorrect and then process the request without a redirect, since
   the invalid request-line might be deliberately crafted to bypass
   security filters along the request chain.

   HTTP does not place a predefined limit on the length of a
   request-line, as described in Section 2.5.  A server that receives a
   method longer than any that it implements SHOULD respond with a 501
   (Not Implemented) status code.
*/

/*
    RFC 7230
    request-target longer than any URI it wishes to parse MUST respond
    with a 414 (URI Too Long) status code (see Section 6.5.12 of
    [RFC7231]).
*/
void                Request::_check_uri(void)
{
    if (_uri.length() == 0)
    {
        std::cerr << REDB << "Empty uri" << RESET << std::endl;
        throw BadRequestException();
    }
    else if (_uri.find_first_of(" \t", 0) != std::string::npos)
    {
        std::cerr << REDB << "whitespace in uri" << RESET << std::endl;
        throw BadRequestException();
    }
}
void                Request::_process_request_line(std::string request_line)
{
    _check_request_line_size(request_line);
    _parse_request_line(request_line);
    _check_method_syntax();
    _check_uri();
    _check_protocol();
    _check_version();
    _check_method_allows_body();
}

// ---------------- Headers  -----------------//

/*
    A server that receives a request header field, or set of fields,
    larger than it wishes to process MUST respond with an appropriate 4xx
    (Client Error) status code.  Ignoring such header fields would
    increase the server's vulnerability to request smuggling attacks
    (Section 9.5).
*/
void                Request::_check_str_us_ascii(std::string &str)
{
    // consume
    if (str.length() == 0)
        std::cout << "";
    // for (size_t i = 0; i < str.length(); ++i)
    // {
    //     if (str[i] < 0 || str[i] > 127)
    //     {
    //         str.erase(i, 1);
    //         i--;
    //     }
    // }
}

/*
    RFC 7230
    field-name     = token
    field-value    = *( field-content / obs-fold )
    field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
    field-vchar    = VCHAR / obs-text

    VCHAR (any visible [USASCII] character).
*/
void                Request::_check_vchar(std::string &str)
{
    _check_str_us_ascii(str);
    for (size_t i = 0; i < str.length(); i++)
    {
        if (std::isprint(str[i]) == 0)
        {
            std::cerr << REDB << "incorrect use of non visible char nb=" << static_cast<int>(str[i]) << ", \
            in field name or field value :<" << str << ">" << RESET << std::endl;
            throw BadRequestException();
        }
    }
}

/*
    field-name     = token
    token          = 1*tchar

    tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
                    / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
                    / DIGIT / ALPHA
                    ; any VCHAR, except delimiters

    Most HTTP header field values are defined using common syntax
    components (token, quoted-string, and comment) separated by
    whitespace or specific delimiting characters.  Delimiters are chosen
    from the set of US-ASCII visual characters not allowed in a token
    (DQUOTE and "(),/:;<=>?@[\]{}").
*/
void                Request::_check_token(std::string &str)
{
    char    dquote = 34; // "
    char    escape = 92; // escape
    size_t  idx0  = str.find_first_of("(),/:;<=>?@[]{}");
    size_t  idx1 = str.find_first_of(dquote);
    size_t  idx2 = str.find_first_of(escape);

    _check_vchar(str);
    if (idx0 != std::string::npos)
    {
        std::cerr << REDB << "delimiter not allowed in token pos("<< idx0 << ") - char (" << str[idx0] << ") in string " << str << ">" << RESET << std::endl;
        throw BadRequestException();
    }
        if (idx1 != std::string::npos)
    {
        std::cerr << REDB << "delimiter not allowed in token pos("<< idx1 << ") - char (" << str[idx1] << ") in string " << str << ">" << RESET << std::endl;
        throw BadRequestException();
    }
        if (idx2 != std::string::npos)
    {
        std::cerr << REDB << "delimiter not allowed in token pos("<< idx2 << ") - char (" << str[idx2] << ") in string " << str << ">" << RESET << std::endl;
        throw BadRequestException();
    }
}

/*
   A server that receives a request header field, or set of fields,
   larger than it wishes to process MUST respond with an appropriate 4xx
   (Client Error) status code.  Ignoring such header fields would
   increase the server's vulnerability to request smuggling attacks
   (Section 9.5).
*/
void                Request::_check_field_name_len(std::string str)
{
    size_t  len = str.length();

    // check for empty key
    if (len == 0)
    {
        std::cerr << REDB << "Header field name empty or separator ':' not found" << RESET << std::endl;
        throw BadRequestException();
    }
    else if (len > MAX_FIELD_NAME_LEN)
    {
        std::cerr << REDB << "Header field name length is too big" << RESET << std::endl;
        throw BadRequestException();
    }
}
void				Request::_check_field_value_len(std::string str)
{
    size_t  len = str.length();

    // a value can be empty check for empty key
    if (len > MAX_FIELD_VALUE_LEN)
    {
        std::cerr << REDB << "Header field value length is too big" << RESET << std::endl;
        throw BadRequestException();
    }
}

/*
    set case-insensitive header values from RFC7230 and RFC7231 to lower case
    - transfer-coding
    - te
    - connection
    - expect
    - content-coding
    - content-type
    - charset
    - vary
    - language-tag
*/
void        Request::_modify_header_values_tolower(void)
{
    // modify header values that must be case-insensitive
    if (get_header_per_key("transfer-coding").length() > 0)
        _headers_map["transfer-coding"] = _convert_tolower(get_header_per_key("transfer-coding"));

     if (get_header_per_key("te").length() > 0)
        _headers_map["te"] = _convert_tolower(get_header_per_key("te"));

     if (get_header_per_key("connection").length() > 0)
        _headers_map["connection"] = _convert_tolower(get_header_per_key("connection"));
     
     if (get_header_per_key("expect").length() > 0)
        _headers_map["expect"] = _convert_tolower(get_header_per_key("expect"));
     
     if (get_header_per_key("content-coding").length() > 0)
        _headers_map["content-coding"] = _convert_tolower(get_header_per_key("content-coding"));

    if (get_header_per_key("charset").length() > 0)
        _headers_map["charset"] = _convert_tolower(get_header_per_key("charset"));
   
    if (get_header_per_key("vary").length() > 0)
        _headers_map["vary"] = _convert_tolower(get_header_per_key("vary"));
    
    if (get_header_per_key("language-tag").length() > 0)
        _headers_map["language-tag"] = _convert_tolower(get_header_per_key("language-tag"));
}



/*
    RFC7230
    A message-body MUST NOT be included in
    a request if the specification of the request method (section 5.1.1)
    does not allow sending an entity-body in requests. A server SHOULD
    read and forward a message-body on any request; if the request method
    does not include defined semantics for an entity-body, then the
    message-body SHOULD be ignored when handling the request.

    DELETE RFC7231
    A payload within a DELETE request message has no defined semantics;
    sending a payload body on a DELETE request might cause some existing
    implementations to reject the request.
*/
void    Request::_check_method_allows_body(void)
{
    // TODO if (_method.compare("DELETE") == 0) uncomment
    // {
    //     // ignore any body senet
    //     _body.clear();
    //     set_content_length(0);
    // }
}





void				Request::_check_mandatory_header_fields(void)
{
	// Host is mandatory otherwise -> 404 Bad Request
	if (get_host().length() == 0)
	{
		std::cout << REDB << "Host field not in request" << RESET << std::endl;
		throw BadRequestException();
	}

    // content length is mandatory in some conditions
    _check_content_length(); // uncomment

}

/*
    RFC 7230
    The message body (if any) of an HTTP message is used to carry the
    payload body of that request or response.  The message body is
    identical to the payload body unless a transfer coding has been
    applied, as described in Section 3.3.1.

    The presence of a message-body in a request is signaled by the
    inclusion of a Content-Length or Transfer-Encoding header field in
    the request's message-headers.

    Content lenght

    A user agent SHOULD send a Content-Length in a request message when
    no Transfer-Encoding is sent and the request method defines a meaning
    for an enclosed payload body.  For example, a Content-Length header
    field is normally sent in a POST request even when the value is 0
    (indicating an empty payload body).
    (e.g., "Content-Length: 42, 42"),
    indicating that duplicate Content-Length header fields have been
    generated or combined by an upstream message processor, then the
    recipient MUST either reject the message as invalid or replace the
    duplicated field-values with a single valid Content-Length field
    containing that decimal value prior to determining the message body
    length or forwarding the message.

    If a message is received with both a Transfer-Encoding and a
        Content-Length header field, the Transfer-Encoding overrides the
        Content-Length.  Such a message might indicate an attempt to
        perform request smuggling (Section 9.5) or response splitting
        (Section 9.4) and ought to be handled as an error.  A sender MUST
        remove the received Content-Length field prior to forwarding such
        a message downstream
        If a valid Content-Length header field is present without
        Transfer-Encoding, its decimal value defines the expected message
        body length in octets.  If the sender closes the connection or
        the recipient times out before the indicated number of octets are
        received, the recipient MUST consider the message to be
        incomplete and close the connection.

        A server MAY reject a request that contains a message body but not a
        Content-Length by responding with 411 (Length Required).
        A server that receives an incomplete request message, usually due to
        a canceled request or a triggered timeout exception, MAY send an
        error response prior to closing the connection.
*/
void    Request::_check_content_length(void)
{
    std::string content_length = get_header_per_key("Content-Length");

    // no content length sent but there is a body => 411 Length Required
    if (content_length.length() == 0 && _body.length() > 0 )
    {
        // is chunked?
		if (get_transfer_encoding().length() == 0 || get_transfer_encoding().compare("chunked") != 0)
		{
			std::cout << REDB << "Content-Length field necessary if body is sent and its transfer enconding is not chunked" << RESET << std::endl;
			throw LengthRequiredException();
		}
    }

    // content length only has digits
    for (size_t i = 0; i < content_length.length(); i++)
    {
        if (std::isdigit(content_length[i]) == 0)
        {
            std::cout << REDB << "Content-Length is not a number" << RESET << std::endl;
		    throw BadRequestException();
        }
    }

    // negative content length is not allowed
    if (content_length.length() != 0 && str2int(content_length) < 0)
    {
    	std::cout << REDB << "Content-Length is a negative number" << RESET << std::endl;
		throw BadRequestException();
    }

    // body size is not the same as the body_size
    else if (content_length.length() != 0 && str2int(content_length) != (int)_body.length()
    && (get_transfer_encoding().length() == 0 || get_transfer_encoding().compare("chunked") != 0))
    {
        std::cout << REDB << "Content-Length does not match the size of the body received. Content-Length is " << content_length << ",body is "  << _body.length() << RESET << std::endl;
        throw BadRequestException();
    }
}

/*
    RFC 7230
    Each header field consists of a case-insensitive field name followed
    by a colon (":"), optional leading whitespace, the field value, and
    optional trailing whitespace.

    A sender MUST NOT generate multiple header fields with the same field
    name in a message unless either the entire field value for that
    header field is defined as a comma-separated list [i.e., #(values)]
    or the header field is a well-known exception (as noted below).

    A recipient MAY combine multiple header fields with the same field
    name into one "field-name: field-value" pair, without changing the
    semantics of the message, by appending each subsequent field value to
    the combined field value in order, separated by a comma.  The order
    in which header fields with the same field name are received is
    therefore significant to the interpretation of the combined field
    value; a proxy MUST NOT change the order of these field values when
    forwarding a message.
*/
void                Request::_parser_header_line(std::string line)
{
 // get key = field name
    std::string key = _extract_until_delimiter(&line, ":");

    // get value = field value
    std::string value = line;

    // check field name size
    _check_field_name_len(key);

    // check field value size
    _check_field_value_len(value);

    // check for vchar and invalid characters for token = field name
    _check_token(key);

    // check visible chars in field value
    _check_vchar(value);

    // remove leading  LWS from value
    _remove_leading_whitespace(value);

    // remove trailing LWS from value
    _remove_trailing_whitespace(value);

    // check if header field name is duplicated or not
    std::map<std::string, std::string>::iterator it = _headers_map.find(key);

    // key does not exist in the the map and can be added
    if (it == _headers_map.end())
    {
        _headers_map[_convert_tolower(key)] = value;
    }
    // key already exist in map
    else
    {
        // check if the repeated field name is   "field-name: field-value" for "field-value, field-value"
        size_t idx_dp = _headers_map[key].find_first_of(":");
        size_t exist_len = _headers_map[key].length();
        if (exist_len > 0 && idx_dp != std::string::npos && idx_dp < exist_len)
        {
            size_t idx_comma = value.find_first_of(",");
            size_t new_len = value.length();
            if (new_len > 0 && idx_comma != std::string::npos && idx_dp < new_len)
            {
                // append new value
                _headers_map[key].append(", ");
                _headers_map[key].append(value);
            }
            else
            {
                std::cerr << REDB << "Invalid duplicated header field-name" << RESET << std::endl;
                throw BadRequestException();
            }
        }
        else
        {
            std::cerr << REDB << "Invalid duplicated header field-name" << RESET << std::endl;
            throw BadRequestException();
        }
    }
}
void                Request::_process_header_line(std::string header_line)
{
    _check_field_value_obs_fold(header_line);
    _parser_header_line(header_line);
}

void                Request::print_headers_map(void)
{
     // create an map iterator element
	std::map<std::string, std::string>::iterator it;

	for (it = _headers_map.begin(); it != _headers_map.end(); it++)
	{
        std::cout << "\tKey:<" << it->first << "> | Value:<" << it->second << ">" << std::endl;
    }
}

void                Request::print_request(void)
{
    std::cout << YELLOW << std::endl;
    std::cout << "-------- Request Line --------" << std::endl;
    std::cout << "\tMethod   <" << get_method() << ">" << std::endl;
    std::cout << "\tUri      <" << get_uri() << ">" << std::endl;
    std::cout << "\tProtocol <" << get_protocol() << ">" << std::endl;
    std::cout << "\tVersion  <" << get_version() << ">" << std::endl;
    std::cout << std::endl;

    std::cout << "-------- Headers --------" << std::endl;
    print_headers_map();
    std::cout << std::endl;

    std::cout << "---------  BODY  -------" << std::endl;
    std::cout << "\tBody " << std::endl << "<" << get_body() << ">" << std::endl;
    std::cout << std::endl;

	std::cout << RESET;
}


// ---------------- Body  -----------------//

void                Request::_process_body(std::string body)
{
  std::string transfer_enconding = get_header_per_key("Transfer-Encoding");

    // consume body
    if (body.length() == 0)
        std::cout << "";


     // if header content len is passed, convert content length to long
    if (get_header_per_key("Content-Length").length() > 0)
    {
        _convert_content_length();
        if (_content_len != _body.length())
        {
            std::cerr << REDB  << "mismatch from Content-Header field value and actual body length received" << RESET << std::endl;
            throw BadRequestException();
        }
        else if (_content_len > MAX_BODY_SIZE)
        {
            std::cerr << REDB << "body is bigger than define max body size in ConfigFile" << RESET << std::endl;
            throw RequestEntityTooLargeException();
        }
    }
    else if (transfer_enconding.length() > 0)
    {
        if (transfer_enconding.compare("chunked") == 0) // uncomment this line
            _process_chunk(_body);
        else
        {
            std::cerr << REDB << "Transfer-Encoding: " << transfer_enconding << ", is not supported" << RESET << std::endl;
            throw BadRequestException();
        }
    }

     // handle file transfer?

    //  _check_body_octet(_body);
}

// ---------------- Chunked Body  -----------------//


// Transfer Encoding
/*
    transfer-coding    = "chunked" ; Section 4.1
                    / "compress" ; Section 4.2.1
                    / "deflate" ; Section 4.2.2
                    / "gzip" ; Section 4.2.3
                    / transfer-extension
    transfer-extension = token *( OWS ";" OWS transfer-parameter )
    registered within the HTTP Transfer Coding registry, as defined in
    Section 8.4.  They are used in the TE (Section 4.3) and
    Transfer-Encoding (Section 3.3.1) header fields.


    A recipient MUST be able to parse the chunked transfer coding
    (Section 4.1) because it plays a crucial role in framing messages
    when the payload body size is not known in advance.

    If any transfer coding
    other than chunked is applied to a request payload body, the sender
    MUST apply chunked as the final transfer coding to ensure that the
    message is properly framed.  If any transfer coding other than
    chunked is applied to a response payload body, the sender MUST either
    apply chunked as the final transfer coding or terminate the message
    by closing the connection.

    For example,

        Transfer-Encoding: gzip, chunked

    indicates that the payload body has been compressed using the gzip
    coding and then chunked using the chunked coding while forming the
    message body.

    chunked-body   = *chunk
                        last-chunk
                        trailer-part
                        CRLF

        chunk          = chunk-size [ chunk-ext ] CRLF
                        chunk-data CRLF
        chunk-size     = 1*HEXDIG
        last-chunk     = 1*("0") [ chunk-ext ] CRLF

        chunk-data     = 1*OCTET ; a sequence of chunk-size octets

    HEXDIG (hexadecimal 0-9/A-F/a-f)

        The chunk-size field is a string of hex digits indicating the size of
    the chunk-data in octets.  The chunked transfer coding is complete
    when a chunk with a chunk-size of zero is received, possibly followed
    by a trailer, and finally terminated by an empty line.

    A recipient MUST ignore unrecognized chunk extensions.  A server
    ought to limit the total length of chunk extensions received in a
    request to an amount reasonable for the services provided, in the
    same way that it applies length limitations and timeouts for other
    parts of a message, and generate an appropriate 4xx (Client Error)
    response if that amount is exceeded.

    4.1.1.  Chunk Extensions

    The chunked encoding allows each chunk to include zero or more chunk
    extensions, immediately following the chunk-size, for the sake of
    supplying per-chunk metadata (such as a signature or hash),
    mid-message control information, or randomization of message body
    size.

        chunk-ext      = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )

        chunk-ext-name = token
        chunk-ext-val  = token / quoted-string
        
        Hence, use of chunk extensions is generally limited
    to specialized HTTP services such as "long polling" (where client and
    server can have shared expectations regarding the use of chunk
    extensions) or for padding within an end-to-end secured connection.
    A recipient MUST ignore unrecognized chunk extensions.  A server
    ought to limit the total length of chunk extensions received in a
    request to an amount reasonable for the services provided, in the
    same way that it applies length limitations and timeouts for other
    parts of a message, and generate an appropriate 4xx (Client Error)
    response if that amount is exceeded.
   

    4.1.3.  Decoding Chunked

    A process for decoding the chunked transfer coding can be represented
    in pseudo-code as:

        length := 0
        read chunk-size, chunk-ext (if any), and CRLF
        while (chunk-size > 0) {
        read chunk-data and CRLF
        append chunk-data to decoded-body
        length := length + chunk-size
        read chunk-size, chunk-ext (if any), and CRLF
        }
        read trailer field
        while (trailer field is not empty) {
        if (trailer field is allowed to be sent in a trailer) {
            append trailer field to existing header fields
        }
        read trailer-field
        }
        Content-Length := length
        Remove "chunked" from Transfer-Encoding
        Remove Trailer from existing header fields

    POST /upload HTTP/1.1
    Host: example.com
    Content-Type: application/octet-stream
    Transfer-Encoding: chunked

    4; param1=value1; param2=value2
    data
    5; param3=value3
    chunked
    0
    Trailer-Header1: value1
    Trailer-Header2: value2

    unchunked:4data5chunked
*/
void                Request::_convert_content_length(void)
{
    set_content_length(_convert_str2size_t(get_header_per_key("Content-Length")));
}

/*

4.1.2.  Chunked Trailer Part

   A trailer allows the sender to include additional fields at the end
   of a chunked message in order to supply metadata that might be
   dynamically generated while the message body is sent, such as a
   message integrity check, digital signature, or post-processing
   status.  The trailer fields are identical to header fields, except
   they are sent in a chunked trailer instead of the message's header
   section.

    trailer-part   = *( header-field CRLF )

    Forbidden trailer headers
    A sender MUST NOT generate a trailer that contains a field necessary
    for message framing (e.g., Transfer-Encoding and Content-Length),
    routing (e.g., Host), request modifiers (e.g., controls and
    conditionals in Section 5 of [RFC7231]), authentication (e.g., see
    [RFC7235] and [RFC6265]), response control data (e.g., see Section
    7.1 of [RFC7231]), or determining how to process the payload (e.g.,
    Content-Encoding, Content-Type, Content-Range, and Trailer).
    When a chunked message containing a non-empty trailer is received,
    the recipient MAY process the fields (aside from those forbidden
    above) as if they were appended to the message's header section.  A
    recipient MUST ignore (or consider as an error) any fields that are
    forbidden to be sent in a trailer, since processing them as if they
    were present in the header section might bypass external security
    filters.

    controls RFC7231 section 5
    Cache-Control     | Section 5.2 of [RFC7234] |
   | Expect            | Section 5.1.1            |
   | Host              | Section 5.4 of [RFC7230] |
   | Max-Forwards      | Section 5.1.2            |
   | Pragma            | Section 5.4 of [RFC7234] |
   | Range             | Section 3.1 of [RFC7233] |
   | TE                | Section 4.3 of [RFC7230] |
   

*/
void                Request::_find_chunk_trailer_headers(std::string &chunk_data, std::string &str)
{
    std::string line;
    if (chunk_data.length() > 0)
        _extract_chunk_trailer_header(chunk_data);
    while (str.length() > 0)
    {
        line = _extract_until_delimiter(&str, "\r\n");
        _extract_chunk_trailer_header(line);
    }
}


void                Request::_extract_chunk_trailer_header(std::string &chunk_trailer_line)
{
     // check if the trailer content in chunk is  "field-name: field-value"
    size_t idx_dp = chunk_trailer_line.find_first_of(":");
    size_t len = chunk_trailer_line.length();
    if (len > 0 && idx_dp != std::string::npos && idx_dp < len)
    {
        if (idx_dp > 0)
        {
            std::string key = _convert_tolower(chunk_trailer_line.substr(0, idx_dp));
            std::string value = chunk_trailer_line.substr(idx_dp + 1);
            // check if it is not a prohibit header
            if (key.compare("transfer-encoding") == 0
                || key.compare("content-length") == 0
                || key.compare("host") == 0
                || key.compare("content-encoding") == 0 
                || key.compare("content-type") == 0
                || key.compare("content-range") == 0
                || key.compare("expect") == 0
                || key.compare("trailer") == 0
                || key.compare("max-forwards") == 0
                || key.compare("pragma") == 0
                || key.compare("range") == 0 
                || key.compare("te") == 0 )
            {
                std::cerr << REDB << "Passed chunk header is forbidden" << RESET << std::endl;
                throw BadRequestException();
            }
            _remove_leading_whitespace(value);
            _remove_trailing_whitespace(value);
            _headers_map[key] = value;
        }
        else
        {
            std::cerr << REDB << "Format error in chunk trailer" << RESET << std::endl;
            throw BadRequestException();
        }
    }
}


void                Request::_process_chunk(std::string str)
{
    size_t          accum_length = 0;
    std::string     accum_body = "";
    std::string     str_chunk_size;
    size_t          chunk_size;
    std::string     chunk_data;

    if (str.rfind("\r\n\r\n") == std::string::npos)
    {
        std::cerr << REDB  << "error format last-chunk terminator CRLF CRLF not found" << RESET << std::endl;
        throw BadRequestException();
    }
    while(str.length() > 0)
    {
        str_chunk_size = _extract_until_delimiter(&str, "\r\n");
        if (str_chunk_size.length() == 0)
        {
            std::cerr << REDB  << "error to retrieve chunk size from request" << RESET << std::endl;
            throw BadRequestException();
        }

        // TODO split here chunk size from chunk-extens...  
        chunk_size = _convert_str2hex(str_chunk_size);

        chunk_data = _extract_until_delimiter(&str, "\r\n");
        // if (chunk_data.length() != chunk_size) // TODO should we remove this check? in case of trailer headers, this will always fail
        // {
        //     std::cerr << REDB  << "chunk data length does not match chunk size" << RESET << std::endl;
        //     throw BadRequestException();
        // }

        // else if (chunk_size == 0) // && str.length() == 0)
        if (chunk_size == 0) // && str.length() == 0)
        {
            // chunk has reached its end
            // new body and new content length
            _body = accum_body;
            set_content_length(accum_length);
            _find_chunk_trailer_headers(chunk_data , str);
            return ;
        }
        accum_length += chunk_size;
        accum_body.append(chunk_data);
    }
    std::cerr << REDB  << "error format last-chunk" << RESET << std::endl;
    throw BadRequestException();
}


// ---------------- Transfer file  -----------------//

/* to be added */


// ---------------- Getters/Setters  -----------------//


std::string         Request::get_header_per_key(std::string header_key) const
{
    if (header_key.length() == 0)
        return ("");
    std::map<std::string, std::string>::const_iterator it = _headers_map.find(_convert_tolower(header_key));

    if (it == _headers_map.end())
    {
        return ("");
    }
    return(it->second);
}

size_t              Request::get_content_length(void)
{
    return (_content_len);
}

void                Request::set_content_length(size_t len)
{
    _content_len = len;
    _headers_map["content-length"] = size_t2str(_content_len);
}

int                 Request::get_error(void) const
{
    return (_error);
}

std::string         Request::get_method(void) const
{
    return (_method);
}

std::string         Request::get_uri(void) const
{
    return (_uri);
}

std::string         Request::get_protocol(void) const
{
    return (_protocol);
}

std::string         Request::get_version(void) const
{
    return (_version);
}

std::string         Request::get_user_agent(void) const
{
    return (get_header_per_key("User-Agent"));
}

std::string         Request::get_host(void) const
{
    return (get_header_per_key("Host"));
}

std::string         Request::get_hostname(void) const
{
    return (get_header_per_key("Hostname"));
}

std::string         Request::get_port(void) const
{
    return (get_header_per_key("Port"));
}

std::string        Request::get_accept_encoding(void) const
{
    return (get_header_per_key("Accept-Encoding"));
}

std::string         Request::get_transfer_encoding(void) const
{
    return (get_header_per_key("Transfer-Encoding"));
}

std::string         Request::get_connection(void) const
{
    return (get_header_per_key("Connection"));
}

std::string         Request::get_cache_control(void) const
{
    return (get_header_per_key("Cache-Control"));
}

std::string         Request::get_date(void) const
{
    return (get_header_per_key("Date"));
}

std::string         Request::get_pragma(void) const
{
    return (get_header_per_key("Pragma"));
}

std::string         Request::get_trailer(void) const
{
    return (get_header_per_key("Trailer"));
}

std::string         Request::get_upgrade(void) const
{
    return (get_header_per_key("Upgrade"));
}

std::string         Request::get_via(void) const
{
    return (get_header_per_key("Via"));
}

std::string         Request::get_warning(void) const
{
    return (get_header_per_key("Warning"));
}

std::string         Request::get_accept_charset(void) const
{
    return (get_header_per_key("Accept-Charset"));
}

std::string         Request::get_accept_language(void) const
{
    return (get_header_per_key("Accept-Language"));
}

std::string         Request::get_accept_authorization(void) const
{
    return (get_header_per_key("Authorization"));
}

std::string         Request::get_except(void) const
{
    return (get_header_per_key("Exception"));
}

std::string         Request::get_from(void) const
{
    return (get_header_per_key("From"));
}

std::string         Request::get_if_match(void) const
{
    return (get_header_per_key("If-Match"));
}

std::string         Request::get_if_modified_since(void) const
{
    return (get_header_per_key("If-Modified-Since"));
}

std::string        Request::get_if_none_match(void) const
{
    return (get_header_per_key("If-None-Match"));
}

std::string         Request::get_if_range(void) const
{
    return (get_header_per_key("If-Range"));
}

std::string         Request::get_if_unmodified_since(void) const
{
    return (get_header_per_key("If-Unmodified-Since"));
}

std::string         Request::get_max_forwards(void) const
{
    return (get_header_per_key("Max-Forwards"));
}

std::string         Request::get_proxy_authorization(void) const
{
    return (get_header_per_key("Proxy-Authorization"));
}

std::string         Request::get_range(void) const
{
    return (get_header_per_key("Range"));
}

std::string         Request::get_referer(void) const
{
    return (get_header_per_key("Referer"));
}

std::string         Request::get_te(void) const
{
    return (get_header_per_key("TE"));
}

std::string		    Request::get_body(void) const
{
    return (_body);
}


/*
    RFC7231
    The "Expect" header field in a request indicates a certain set of
    behaviors (expectations) that need to be supported by the server in
    order to properly handle this request.  The only such expectation
    defined by this specification is 100-continue.

        Expect  = "100-continue"

    The Expect field-value is case-insensitive.

    A server that receives an Expect field-value other than 100-continue
    MAY respond with a 417 (Expectation Failed) status code to indicate
    that the unexpected expectation cannot be met.
*/


// BODY


/*
    message-body = *OCTET
    OCTET   =  %x00-FF (any 8-bit sequence of data)
*/

// void    _check_body_octet(std::string str)
// {
//     for (size_t i = 0; i < str.length(); i++)
//     {
//         if (str[i] < 0 && str[i] > 255 )
//         {
//             std::cerr << REDB << "body OCTET characters is not in the range of x00-FF" << RESET << std::endl;
//             throw BadRequestException();
//         }
//     }
// }

/* finish void                Request::_process_body(std::string body)
*/


/*


    RFC 2616 - 4.2 Message Headers
    Field names
    are case-insensitive 
    An HTTP/1.1 user agent MUST NOT preface
    or follow a request with an extra CRLF.  If terminating the request
    message body with a line-ending is desired, then the user agent MUST
    count the terminating CRLF octets as part of the message body length
    In the interest of robustness, a server that is expecting to receive
    and parse a request-line SHOULD ignore at least one empty line (CRLF)
    received prior to the request-line.

   Although the line terminator for the start-line and header fields is
   the sequence CRLF, a recipient MAY recognize a single LF as a line
   terminator and ignore any preceding CR.

   When a server listening only for HTTP request messages, or processing
   what appears from the start-line to be an HTTP request message,
   receives a sequence of octets that does not match the HTTP-message
   grammar aside from the robustness exceptions listed above, the server
   SHOULD respond with a 400 (Bad Request) response.

*/
//implement

/*
    RFC 7230
    The normal procedure for parsing an HTTP message is to read the
   start-line into a structure, read each header field into a hash table
   by field name until the empty line, and then use the parsed data to
   determine if a message body is expected.  If a message body has been
   indicated, then it is read as a stream until an amount of octets
   equal to the message body length is read or the connection is closed.

    A sender MUST NOT send whitespace between the start-line and the
   first header field.  A recipient that receives whitespace between the
   start-line and the first header field MUST either reject the message
   as invalid or consume each whitespace-preceded line without further
   processing of it (i.e., ignore the entire line, along with any
   subsequent lines preceded by whitespace, until a properly formed
   header field is received or the header section is terminated).

   The presence of such whitespace in a request might be an attempt to
   trick a server into ignoring that field or processing the line after
   it as a new request, either of which might result in a security
   vulnerability if other implementations within the request chain
   interpret the same message differently.  Likewise, the presence of
   such whitespace in a response might be ignored by some clients or
   cause others to cease parsing.

*/

/*
    chunked with exten and trailer
    check if chunnk was sent in enconding but message body was not chunked

*/

/*
    is void                Request::_check_str_us_ascii(std::string &str) required?
    range is always false

*/


/*
A sender MUST NOT generate an "http" URI with an empty host identifier. A recipient that processes such a URI reference MUST reject it as invalid.

If the host identifier is provided as an IP address, the origin server is the listener (if any) on the indicated TCP port at that IP address.
If host is a registered name, the registered name is an indirect identifier for use with a name resolution service, such as DNS, to find an address
for that origin server. If the port subcomponent is empty or not given, TCP port 80 (the reserved port for WWW services) is the default.
*/