

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h> // open
#include <map>

#include "Request.hpp"

/*
esponse

   After receiving and interpreting a request message, a server responds
   with an HTTP response message.

       Response      = Status-Line               ; Section 6.1
                       *(( general-header        ; Section 4.5
                        | response-header        ; Section 6.2
                        | entity-header ) CRLF)  ; Section 7.1
                       CRLF
                       [ message-body ]          ; Section 7.2

Status-Line

   The first line of a Response message is the Status-Line, consisting
   of the protocol version followed by a numeric status code and its
   associated textual phrase, with each element separated by SP
   characters. No CR or LF is allowed except in the final CRLF sequence.

    Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

*/

class Response
{
	private:
		std::string		_html_content;
		size_t			_html_content_size;
		std::string		_status_line;
		std::string		_response;
		char 			**_envp;
		std::map<std::string, std::string> &_error_page_map;


		// Response(Response const &src);
		Response	&operator=(Response const &rhs);
		int			_read_file_data(Request const &req);
		int			_create_status_line(void);
		void 		_setup_response(char *env[]);
		void		_parse_response(Request const &req);


	public:
		// Response(void);
		Response(std::map<std::string, std::string> &error_page_map);
		// Response(char  *env[]);
		~Response(void);
		void			create_response(Request const &req, char *env[]);

		std::string		get_response(void) const;
		std::string		get_html_content(void) const;
		size_t			get_html_size(void) const;
		// void			set_error_page_map(std::map<std::string, std::string> *error_page_map);


};

#endif