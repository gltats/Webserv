/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 21:37:17 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/24 20:53:16 by mgranero         ###   ########.fr       */
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
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <map>
#include <stdlib.h>

#include "Request.hpp"
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
		Request								&_request;

		// int									_status;
		std::string							_html_content;
		size_t								_html_content_size;
		std::string							_status_line;
		std::string							_response;

		char 								**_envp;
		std::map<std::string, std::string>	_error_page_map;
		std::map<std::string, std::string>	_response_status_map;

		int 								_fd_stdin;
		int 								_fd_stdout;
		int									_fd_pipe[2];

		bool								_is_cgi;

		// Response(Response const &src);
		Response							&operator=(Response const &rhs);
		int									_read_file_data(Request const &req);
		int									_create_status_line(void);
		void 								_setup_response(void);
		void								_parse_response(Request const &req);
		void								_read_from_cgi(File &file);


	public:
		Response(ConfigParser &configParser, Request &request, char *env[]);

		~Response(void);
		void								create_response(int server_id);
		void 								process_cgi(char const *buffer, int buffer_size);
		bool								get_is_cgi(void);

		std::string							get_response(void) const;
		std::string							get_html_content(void) const;
		size_t								get_html_size(void) const;
		std::map<std::string, std::string>	get_error_page_map(void) const;
		std::map<std::string, std::string>	get_response_status_map(void) const;
		int									get_fd_stdin(void) const;
		int									get_fd_pipe_0(void) const;


};

#endif