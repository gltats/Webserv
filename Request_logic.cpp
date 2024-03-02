/*
    ASK TATIANA if we need to support GET all the time
    The methods GET
   and HEAD MUST be supported by all general-purpose servers. All other
   methods are OPTIONAL; however, if the above methods are implemented,
   they MUST be implemented with the same semantics as those specified
   in section 9.

*/

#include <iostream>
#include <map>
#include <stdlib.h>
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

std::map<std::string, std::string> _headers_map;

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


void _remove_leading_LWS(std::string &str)
{
    size_t  idx;

    while (1)
    {
        // try to find option 1: CRLFSP
        idx = str.find("\r\n ", 0, 3);
        if (idx == std::string::npos || idx != 0)
        {
            // try to find option 1: CRLFHT
            idx = str.find("\r\n\t", 0, 3);
            if (idx == std::string::npos)
            {
                // no LWS found -> nothing to remove
                return ;
            }  
        }
        // only remove leading LWS (before the first non LWS)
        if (idx != 0)
            return ;
        // remove leading LWS
        str.erase(0, 3);
    }
}

void _remove_trailing_LWS(std::string &str)
{
    size_t  idx;

    while (1)
    {
        // try to find option 1: CRLFSP
        idx = str.rfind("\r\n ", str.length() - 1, 3);
        if (idx == std::string::npos || idx != str.length() - 3)
        {
            // try to find option 1: CRLFHT
            idx = str.rfind("\r\n\t",  str.length() - 1, 3);
            if (idx == std::string::npos)
            {
                // no LWS found -> nothing to remove
                return ;
            }  
        }
        // only remove leading LWS (before the first non LWS)
        if (idx !=  str.length() - 3)
            return ;
        // remove trailing LWS
        str.erase(str.length() - 3, 3);
    }
}

//"B\r\n \r\n E"
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
        idx_last_match = idx;
    }
}

/*
    RFC 2616 - Header Field Value
    LWS            = [CRLF] 1*( SP | HT )
    
    Such leading or trailing LWS MAY be
    removed without changing the semantics of the field value. Any LWS
    that occurs between field-content MAY be replaced with a single SP
    before interpreting the field value or forwarding the message
    downstream
*/
void   _remove_LWS(std::string &str)
{
    _remove_leading_LWS(_buffer);
    _remove_trailing_LWS(_buffer);
    _replace_middle_LWS(_buffer);
}

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

*/
//implement check 

/*
    An
   origin server SHOULD return the status code 405 (Method Not Allowed)
   if the method is known by the origin server but not allowed for the
   requested resource, and 501 (Not Implemented) if the method is
   unrecognized or not implemented by the origin server.

*/
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
        //fix with specs!
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

/*
    RFC 2616
    HTTP/1.1 header field values can be folded onto multiple lines if the
   continuation line begins with a space or horizontal tab. All linear
   white space, including folding, has the same semantics as SP. A
   recipient MAY replace any linear white space with a single SP before
   interpreting the field value or forwarding the message downstream.

       LWS            = [CRLF] 1*( SP | HT )
*/
/*
    RFC 2616
    The field value MAY be preceded by any amount
    of LWS, though a single SP is preferred. Header fields can be
    extended over multiple lines by preceding each extra line with at
    least one SP or HT.

    message-header = field-name ":" [ field-value ]
       field-name     = token
       field-value    = *( field-content | LWS )
       field-content  = <the OCTETs making up the field-value
                        and consisting of either *TEXT or combinations
                        of token, separators, and quoted-string>
    The field-content does not include any leading or trailing LWS:
   linear white space occurring before the first non-whitespace
   character of the field-value or after the last non-whitespace
   character of the field-value. Such leading or trailing LWS MAY be
   removed without changing the semantics of the field value. Any LWS
   that occurs between field-content MAY be replaced with a single SP
   before interpreting the field value or forwarding the message
   downstream.

   The order in which header fields with differing field names are
   received is not significant. However, it is "good practice" to send
   general-header fields first, followed by request-header or response-
   header fields, and ending with the entity-header fields.

   Multiple message-header fields with the same field-name MAY be
   present in a message if and only if the entire field-value for that
   header field is defined as a comma-separated list [i.e., #(values)].
   It MUST be possible to combine the multiple header fields into one
   "field-name: field-value" pair, without changing the semantics of the
   message, by appending each subsequent field-value to the first, each
   separated by a comma. The order in which header fields with the same
   field-name are received is therefore significant to the
   interpretation of the combined field value, and thus a proxy MUST NOT
   change the order of these field values when a message is forwarded.

*/

/*
    rfc 822
     3.1.1.  LONG HEADER FIELDS
     
        Each header field can be viewed as a single, logical  line  of
        ASCII  characters,  comprising  a field-name and a field-body.
        For convenience, the field-body  portion  of  this  conceptual
        entity  can be split into a multiple-line representation; this
        is called "folding".  The general rule is that wherever  there
        may  be  linear-white-space  (NOT  simply  LWSP-chars), a CRLF
        immediately followed by AT LEAST one LWSP-char may instead  be
        inserted.  Thus, the single line
     
            To:  "Joe & J. Harvey" <ddd @Org>, JJV @ BBN
     
        can be represented as:
     
            To:  "Joe & J. Harvey" <ddd @ Org>,
                    JJV@BBN
     
        and
     
            To:  "Joe & J. Harvey"
                            <ddd@ Org>, JJV
             @BBN
     
        and
     
            To:  "Joe &
             J. Harvey" <ddd @ Org>, JJV @ BBN

*/
void    check_headers_map(void)
{
     // create an map iterator element
	std::map<std::string, std::string>::iterator it;

	for (it = _headers_map.begin(); it != _headers_map.end(); it++)
	{
        if (it->first.length() == 0)
        {
            std::cerr << REDB << "Key empty" << RESET << std::endl;
            throw BadRequestException();
        }
        else if (it->second.length() > 0 && std::isprint(it->second[0]) == 0)
        {
            std::cerr << REDB << "Value <" << it->second << "> does not start with a printable character" << RESET << std::endl;
            throw BadRequestException();
        }
    } 
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


/*
    RFC 2616 - 4.2 Message Headers
    Field names
   are case-insensitive

*/
//implement

void    response(void)
{
    std::cout <<  "initial buffer: <" << _buffer << ">" << std::endl;
    std::cout << std::endl;
   
    _remove_LWS(_buffer);

    std::cout <<  "final buffer: <" << _buffer << ">" << std::endl;
    std::cout << std::endl;

    // split_headers_body();

    // std::string line;
    // // parse first line -> request line
    // line = extract_until_delimiter(&_headers, "\r\n");
    // parse_request_line(line);

    // while (1)
    // {
    //     line.clear();
    //     line = extract_until_delimiter(&_headers, "\r\n");
    //     if (line.length() == 0)
    //         break; 
    //     parser_header_line(line);
    // }


    // try
    // {
    //     check_method();
    //     check_uri();
    //     check_protocol();
    //     check_version();
    //     check_headers_map();
    // }
    // catch(const MethodNotAllowedException& e)
    // {
    //     std::cerr << REDB << e.what() << RESET << std::endl;
    //     exit (2);
    // }
    // catch(const BadRequestException& e)
    // {
    //     std::cerr << REDB << e.what() << RESET << std::endl;
    //     exit (2);
    // }
    // catch(const HTTPVersionNotSupportedException& e)
    // {
    //     std::cerr << REDB << e.what() << RESET << std::endl;
    //     exit (2);
    // }
    // catch(const RequestURITooLongException& e)
    // {
    //     std::cerr << REDB << e.what() << RESET << std::endl;
    //     exit (2);
    // }
    

    // std::cout << std::endl;
    // std::cout << "PARSED :" << std::endl;
    // std::cout << "method   <" << _method << ">" << std::endl;
    // std::cout << "uri      <" << _uri << ">" << std::endl;
    // std::cout << "protocol <" << _protocol << ">" << std::endl;
    // std::cout << "version  <" << _version << ">" << std::endl;

    // std::cout << "-------- Headers --------" << std::endl;
    // print_headers_map();

    // std::cout << "---------GET HEADER VALUE-------" << std::endl;
    // std::cout << "Existing Key 'Host' value is : <" << get_header_per_key("Host") << ">" << std::endl;
    // std::cout << "Non-Existing Key 'Lasagna' value is : <" << get_header_per_key("Lasagna") << ">" << std::endl;

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
    //tests
    //_buffer = "";


   // _buffer = "GET /html/basic.html HTTP/1.1\r\nHost: Maira\r\nContent-Lenght: 12\r\nTransfer-Encoding: chunked\r\nAccept: enconding\r\n\r\nMozilla i am here\nSomething written\nDeveloper Network";
    _buffer = "B\r\n \r\n E";
    //_buffer = "\r\n Before there was a LWS to be remove\r\n\tand here too\r\n \r\n \r\n\t\r\n \r\n\tbefore this there were 5x them";

    response();
    return (0);
}