/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 21:37:17 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/29 20:30:28 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OS_ENUM
#define OS_ENUM
	enum
	{
		MAC,
		LINUX
	};
#endif


# if defined (__linux__)
		#define OS_PATH LINUX
#endif

# if defined (__mac__)
	#define OS_PATH MAC
#endif


#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <fstream>

#include <sstream>
#include <unistd.h>
#include <fcntl.h> // open
#include <map>
#include <stdlib.h> // for exit

#include "Request.hpp"
#include "map.hpp"
#include "library.hpp"



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

		ConfigParser 						&_configParser;

		// int									_status;
		std::string							_html_content;
		size_t								_html_content_size;
		std::string							_status_line;
		std::string							_response;
		char 								**_envp;
		std::map<std::string, std::string>	_error_page_map;
		std::map<std::string, std::string>	_response_status_map;

		// Response(Response const &src);
		Response							&operator=(Response const &rhs);
		int									_read_file_data(Request const &req);
		int									_create_status_line(void);
		void 								_setup_response(char *env[]);
		void								_parse_response(Request const &req);


	public:
		Response(int server_index, ConfigParser &configParser, Request &_request);

		

		~Response(void);
		void								create_response(Request const &req, char *env[]);

		std::string							get_response(void) const;
		std::string							get_html_content(void) const;
		size_t								get_html_size(void) const;
		std::map<std::string, std::string>	get_error_page_map(void) const;
		std::map<std::string, std::string>	get_response_status_map(void) const;

};

#endif