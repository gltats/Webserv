/*
    ASK TATIANA if we need to support GET all the time
    The methods GET
   and HEAD MUST be supported by all general-purpose servers. All other
   methods are OPTIONAL; however, if the above methods are implemented,
   they MUST be implemented with the same semantics as those specified
   in section 9.

*/

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

#include <iostream>
#include <map>
#include <stdlib.h>
#include "includes/color_code.hpp"

// value to trigger "Request-URI Too Long" status 414
#define MAX_REQUEST_LINE_LEN 8000

#define MAX_FIELD_NAME_LEN 256
#define MAX_FIELD_VALUE_LEN 512

#define MAX_BODY_SIZE 2

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

size_t      _content_len;

int         _error = 0;

std::map<std::string, std::string> _headers_map;


class MethodNotAllowedException: public std::exception
{
	public:
		virtual const char* what() const throw()
        {
            return ("Exception: Status 405, Method not allowed in Config File."); 
        }
};


class UnsupportedMediaTypeException: public std::exception
{
	public:
		virtual const char* what() const throw()
        {
            return ("Exception: Status 415, Unsupported Media Type."); 
        }
};

class RequestEntityTooLargeException: public std::exception
{
	public:
		virtual const char* what() const throw()
        {
            return ("Exception: Status 413, Request Entity Too Large."); 
        }
};


class NotImplemented: public std::exception
{
	public:
		virtual const char* what() const throw()
        {
            return ("Exception: Status 501, Not Implemented. Webserv supports GET, POST and DELETE."); 
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
void _remove_leading_whitespace(std::string &str)
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

void _remove_trailing_whitespace(std::string &str)
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

// header value folding is deprecated "B\r\n \r\n E"

/*
void _replace_middle_LWS(std::string &str)
{
    size_t      idx;

    size_t      idx_next_match;

    while (1)
    {
        // try to find option 1: CRLFSP
        idx = str.find("\r\n ", 0 , 3);
        if (idx == std::string::npos)
        {
            // CR + LF + SP not found, try option 2
            // try to find option 1: CRLFHT
            idx = str.find("\r\n\t", 0);
            if (idx == std::string::npos)
            {
                // no LWS found -> nothing to remove
                return ;
            }  
        }
        std::cout << "idx is " << idx << std::endl;
        
        // replace LWS per Space unless there multiple LWS after each other, 
        // than replace all for one LWS and just delete other LWS

        idx_next_match = str.find("\r\n ", idx + 3, 3);
        std::cout << "idx_next_match is " << idx_next_match << std::endl;
        if ((idx_next_match == std::string::npos) || (idx_next_match != idx + 3))
        {
            std::cout << "replace idx " << idx << std::endl;
            str.replace(idx, 3, " ");
        }
        else
        {
            str.erase(idx, 3);
            std::cout << "erased idx " << idx << " up to " << idx + 3 << std::endl;
        }
         std::cout << "string is <" << str << ">" << std::endl;
         idx_next_match = idx;
    }
}
*/


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
    if (_buffer.length() == 0)
    {
        std::cerr << REDB << "Empty request" << RESET << std::endl;
        throw BadRequestException();
    }
    else if (_buffer.find( "\r\n\r\n") == std::string::npos)
    {
        std::cerr << REDB << "Empty line indicating the end of the header section not found" << RESET << std::endl;
        throw BadRequestException();
    }
    std::string copy_buffer = _buffer;
    _headers = extract_until_delimiter(&copy_buffer, "\r\n\r\n");
    _body = copy_buffer;
}

// REQUEST_START_LINE

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

*/


/*
    RFC7230
    Various ad hoc limitations on request-line length are found in
    practice.  It is RECOMMENDED that all HTTP senders and recipients
    support, at a minimum, request-line lengths of 8000 octets.
*/
void    check_request_line_size(std::string str)
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

void    parse_request_line(std::string request_line)
{
    _method = extract_until_delimiter(&request_line, " ");
    _uri = extract_until_delimiter(&request_line, " ");   
    _protocol = extract_until_delimiter(&request_line, "/");  
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
void    check_method(void)
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
    else if ((_method.compare("GET") == 0 && _allow_GET == false)
        || (_method.compare("POST") == 0 && _allow_POST == false)
        || (_method.compare("DELETE") == 0 && _allow_DELETE == false))
    {
        std::cerr << REDB << "Method <" << _method << ">" << RESET << std::endl;
        throw MethodNotAllowedException();
    }
    std::cout << CYAN << "Valid Method <" << _method << ">" << RESET << std::endl;
}

void    check_protocol(void)
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

void    check_version(void)
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
void    check_uri(void)
{
    size_t  len = _uri.length();
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
    std::cout << CYAN << "URI Version : " << _uri << RESET << std::endl;
}

void _process_request_line(std::string request_line)
{
    check_request_line_size(request_line);
    parse_request_line(request_line);
    check_method();
    check_uri();
    check_protocol();
    check_version();
}

// HEADERS


/*
    A server that receives a request header field, or set of fields,
    larger than it wishes to process MUST respond with an appropriate 4xx
    (Client Error) status code.  Ignoring such header fields would
    increase the server's vulnerability to request smuggling attacks
    (Section 9.5).
*/
void    _check_str_us_ascii(std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] < 0 || str[i] > 127)
        {
            str.erase(i, 1);
            i--;
        }    
    }
}

/*
    RFC 7230
    field-name     = token
    field-value    = *( field-content / obs-fold )
    field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
    field-vchar    = VCHAR / obs-text

    VCHAR (any visible [USASCII] character).
*/
void    _check_vchar(std::string &str)
{
    _check_str_us_ascii(str);
    for (size_t i; i < str.length(); i++)
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


void    _check_token(std::string &str)
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
void    _check_field_name_len(std::string str)
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


void    _check_field_value_len(std::string str)
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
void    _parser_header_line(std::string line)
{
    // get key = field name
    std::string key = extract_until_delimiter(&line, ":");

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
        _headers_map[key] = value;
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
void _check_field_value_obs_fold(std::string field_value)
{
   if (field_value[0] == ' ' || field_value[0] == '\t')
   {
        std::cerr << REDB << "whitespace in header field name, or deprecated field value \
        fold or unsupported Content-Type message/http" << RESET << std::endl;
        throw UnsupportedMediaTypeException();
   }
}


void _process_header_line(std::string header_line)
{
    _check_field_value_obs_fold(header_line);
    _parser_header_line(header_line);

}

/*
 The presence of a message-body in a request is signaled by the
   inclusion of a Content-Length or Transfer-Encoding header field in
   the request's message-headers. A message-body MUST NOT be included in
   a request if the specification of the request method (section 5.1.1)
   does not allow sending an entity-body in requests. 
*/
// check body
//implement 

void    print_headers_map(void)
{
     // create an map iterator element
	std::map<std::string, std::string>::iterator it;

	for (it = _headers_map.begin(); it != _headers_map.end(); it++)
	{
        std::cout << "Key:<" << it->first << "> | Value:<" << it->second << ">" << std::endl;
    } 
}

std::string    get_header_per_key(std::string header_key)
{
    std::map<std::string, std::string>::iterator it = _headers_map.find(header_key);

    if (it == _headers_map.end())
    {
        return ("");
    }
    return(it->second);
}

// BODY

/*
    RFC 7230
      The message body (if any) of an HTTP message is used to carry the
   payload body of that request or response.  The message body is
   identical to the payload body unless a transfer coding has been
   applied, as described in Section 3.3.1.

    message-body = *OCTET

      The rules for when a message body is allowed in a message differ for
   requests and responses.

      The presence of a message body in a request is signaled by a
   Content-Length or Transfer-Encoding header field.  Request message
   framing is independent of method semantics, even if the method does
   not define any use for a message body.


*/

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


size_t  get_content_length(void)
{
    return (_content_len);
}

int    get_error(void)
{
    return (_error);
}

void     _convert_content_length(void)
{
    char*           endptr;
    std::string     str_content_len = get_header_per_key("Content-Length");

    if (str_content_len.length() == 0)
        _content_len = 0;
    else if (str_content_len.length() > 0 && str_content_len[0] == '0')
        _content_len = 0;
    else
    {
        _content_len = std::strtol(str_content_len.c_str(), &endptr, 10);
        if (_content_len == 0 || std::strlen(endptr) > 0)
        {
            std::cerr << REDB << "error to convert content length from string to long" << RESET << std::endl;
            throw BadRequestException();
        }
    }
}

void    _process_body(std::string body)
{
     std::string transfer_enconding = get_header_per_key("Transfer-Encoding");

     // if header content len is passed, convert content length to long
    if (get_header_per_key("Content-Length").length() > 0)
    {
        _convert_content_length();
        size_t _content_len = get_content_length();
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
        // handle chunked


        // handle file transfer?
    }    
    //  _check_body_octet(_body);
}

/*
    RFC 2616 - 4.2 Message Headers
    Field names
   are case-insensitive

*/
//implement

void    request(void)
{

    std::string line;

    std::cout <<  "received request: <" << _buffer << ">" << std::endl;
    std::cout << std::endl;

    try
    {
        split_headers_body();

        // parse first line -> request line
        line = extract_until_delimiter(&_headers, "\r\n");
        _process_request_line(line);       

        while (1)
        {
            line.clear();
            line = extract_until_delimiter(&_headers, "\r\n");
            if (line.length() == 0)
                break; 
            _process_header_line(line);
        }

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

    catch(const NotImplemented& e)
    {
        _error = 501;
        std::cerr << REDB << e.what() << RESET << std::endl;
    }    
    
    if (get_error() != 0)
        exit(_error);

    
    // For testing and visualization of parsing results

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

    std::cout << "---------  BODY  -------" << std::endl;
    std::cout << "Body <" << _body << ">" << std::endl; 

}



/*
    Multiple header keys
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

int     main(void)
{
    //tests
   // _buffer = "";

    // _buffer = "GET / HTTP/1.1\r\nHost: Leila : John\r\nHost: 123123, 12313d\r\n\r\n";
     _buffer = "GET / HTTP/1.1\r\nHost:Maira\r\nContent-Length: 2\r\n\r\nOi";

    //  _buffer = "GET / HTTP/1.1\r\nHost:Maira\r\nContent-Length: 12\r\nTransfer-Encoding: chunked\r\nAccept: enconding\r\n\r\nMozilla i am here\nSomething written\nDeveloper Network";
    // _buffer = "B\r\n \r\n E";
    //_buffer = "\r\n Before there was a LWS to be remove\r\n\tand here too\r\n \r\n \r\n\t\r\n \r\n\tbefore this there were 5x them";

    request();
    return (0);
}