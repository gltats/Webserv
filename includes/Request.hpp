/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 20:32:57 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/14 16:46:21 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>
#include <stdlib.h>
#include <cstring>
#include "color_code.hpp"
#include "library.hpp"
#include "ConfigParser.hpp"

#define MAX_REQUEST_LINE_LEN 8000
#define MAX_FIELD_NAME_LEN 256
#define MAX_FIELD_VALUE_LEN 512
#define MAX_BODY_SIZE 2

class Connection;

class Request
{
    private:
        Connection          &_connection;
        int                 _server_id;
        std::string         _body;
        std::string         _headers;

        std::string         _method;
        std::string         _uri;
        std::string         _protocol;
        std::string         _version;

        size_t              _content_len;
        int                 _error;

        bool                _allow_GET; // get from configParser
        bool                _allow_POST; // get from configParser
        bool                _allow_DELETE; // get from configParser


        std::map<std::string, std::string> _headers_map;

        void                _cleanMemory(void);
        void                _remove_leading_whitespace(std::string &str);
        void                _remove_trailing_whitespace(std::string &str);
        std::string         _extract_until_delimiter(std::string *str, std::string delimiter);
        void                _check_field_value_obs_fold(std::string field_value);
        void                _split_headers_body(std::string &buffer);

        size_t              _convert_str2size_t(std::string str); // move to library convertions
        size_t              _convert_str2hex(std::string str); // move to library convertions

        void                _identify_server(void);

        // Request Start Line
        void                _check_request_line_size(std::string str);
        void                _parse_request_line(std::string request_line);
		void               	_check_method_syntax(void);
		void				_check_allowed_method(void);
        void                _check_protocol(void);
        void                _check_version(void);
        void                _check_uri(void);
        void                _process_request_line(std::string request_line);

        // Headers
        void                _check_str_us_ascii(std::string &str);
        void                _check_vchar(std::string &str);
        void                _check_token(std::string &str);
        void                _check_field_name_len(std::string str);
        void                _check_field_value_len(std::string str);
        void                _parser_header_line(std::string line);
        void                _process_header_line(std::string header_line);
		void				_check_mandatory_header_fields(void);
		void				_split_host_in_hostname_port(void);
		void				_check_valid_port(void);
		void				_check_valid_hostname(void);

        // Body
        void                _process_body(std::string body);


        // Chunked Body
        void                _convert_content_length(void);
        void                _process_chunk(std::string str);


    public:
        Request(Connection &connection);
        ~Request(void);

        void                parse_request(char const *buffer);

        void                print_request(void); // for debugging
        void                print_headers_map(void); // for debugging

        size_t              get_content_length(void);
        void                set_content_length(size_t len);

        std::string		    get_body(void) const;

        std::string         get_method(void) const;
        std::string         get_uri(void) const;
        std::string         get_protocol(void) const;
        std::string         get_version(void) const;

        std::string         get_user_agent(void) const;
        std::string         get_host(void) const;
		std::string			get_hostname(void) const;
		std::string			get_port(void) const;
        std::string         get_accept_encoding(void) const;
        std::string         get_transfer_encoding(void) const;
        std::string         get_connection(void) const;
        std::string         get_cache_control(void) const;
        std::string         get_date(void) const;
        std::string         get_pragma(void) const;
        std::string         get_trailer(void) const;
        std::string         get_upgrade(void) const;
        std::string         get_via(void) const;
        std::string         get_warning(void) const;
        std::string         get_accept_charset(void) const;
        std::string         get_accept_language(void) const;
        std::string         get_accept_authorization(void) const;
        std::string         get_except(void) const;
        std::string         get_from(void) const;
        std::string         get_if_match(void) const;
        std::string         get_if_modified_since(void) const;
        std::string         get_if_none_match(void) const;
        std::string         get_if_range(void) const;
        std::string         get_if_unmodified_since(void) const;
        std::string         get_max_forwards(void) const;
        std::string         get_proxy_authorization(void) const;
        std::string         get_range(void) const;
        std::string         get_referer(void) const;
        std::string         get_te(void) const;


        // To access other Headers which getters are not here (incl. Customs),
        // use get_header_per_key and pass the Header name, if it was
        // received, it will return a string,∫ otherwise ""
        std::string         get_header_per_key(std::string const &header_key) const;

        int                 get_error(void) const;
};

/* move to the webserv exceptions file */
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

#endif

