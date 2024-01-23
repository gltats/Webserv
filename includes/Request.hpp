
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <unistd.h>
#include <strstream>

#include "color_code.hpp"

#define		CR	13
#define		LF	10
#define		SP	32
#define		HT	9


/*

   Request

	Format Overview:

        Request       = Request-Line              ; Section 5.1
                        *(( general-header        ; Section 4.5
                         | request-header         ; Section 5.3
                         | entity-header ) CRLF)  ; Section 7.1
                        CRLF
                        [ message-body ]          ; Section 4.3

	Details:

	Request-Line   = Method SP Request-URI SP HTTP-Version CRLF

	general-header =  Cache-Control            ; Section 14.9
					| Connection               ; Section 14.10
					| Date                     ; Section 14.18
					| Pragma                   ; Section 14.32
					| Trailer                  ; Section 14.40
					| Transfer-Encoding        ; Section 14.41
					| Upgrade                  ; Section 14.42
					| Via                      ; Section 14.45
					| Warning                  ; Section 14.46

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


class Request
{
	private:
		int				_request_status;

		std::string		_method;
		std::string		_uri;
		std::string		_protocol_version;
		std::string		_user_agent;
		std::string		_accept;
		std::string		_host;
		std::string		_accept_encoding;
		std::string		_connection;
		std::string		_cache_control;

		// int				_content_lenght;

		int				_parse_request_line(std::string buffer);
		int				_parser_request_header(std::string buffer);
		int				_parser_general_header(std::string buffer);
		std::string		_get_key_value(std::string buffer, std::string needle);


		Request			&operator=(Request const &rhs);
		Request(Request const &src);
	public:
		Request(void);
		~Request(void);
		int				get_request_status(void) const;
		std::string		get_uri(void) const;
		void			read_request(char const *request_buffer);
		void			print_request(void) const;

};


#endif