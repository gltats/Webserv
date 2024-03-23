/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 21:53:38 by mgranero          #+#    #+#             */
/*   Updated: 2024/03/23 16:19:17 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
	html 3027 bytes
	index image 302743 bytes
	file content is size 302743 (302743)
*/


/*

	HTTP/1.1 defines the sequence CR LF as the end-of-line marker for all
   protocol elements except the entity-body (see appendix 19.3 for
   tolerant applications). The end-of-line marker within an entity-body
   is defined by its associated media type, as described in section 3.7.

       CRLF           = CR LF

   HTTP/1.1 header field values can be folded onto multiple lines if the
   continuation line begins with a space or horizontal tab. All linear
   white space, including folding, has the same semantics as SP. A
   recipient MAY replace any linear white space with a single SP before
   interpreting the field value or forwarding the message downstream.

       LWS            = [CRLF] 1*( SP | HT )

	    [rule]
      Square brackets enclose optional elements; "[foo bar]" is
      equivalent to "*1(foo bar)".

	The TEXT rule is only used for descriptive field contents and values
   that are not intended to be interpreted by the message parser. Words
   of *TEXT MAY contain characters from character sets other than ISO-
   8859-1 [22] only when encoded according to the rules of RFC 2047
   [14].

       TEXT           = <any OCTET except CTLs,
                        but including LWS>

	legend:
 	   CR             = <US-ASCII CR, carriage return (13)>
       LF             = <US-ASCII LF, linefeed (10)>
       SP             = <US-ASCII SP, space (32)>
       HT             = <US-ASCII HT, horizontal-tab (9)>

*/


/*
  generic-message = start-line
                          *(message-header CRLF)
                          CRLF
                          [ message-body ]
        start-line      = Request-Line | Status-Line

	 HTTP/1.1 defines the sequence CR LF as the end-of-line marker for all
   protocol elements except the entity-body (see appendix 19.3 for
   tolerant applications). The end-of-line marker within an entity-body
   is defined by its associated media type, as described in section 3.7.
All HTTP date/time stamps MUST be represented in Greenwich Mean Time
   (GMT), without exception. For the purposes of HTTP, GMT is exactly
   equal to UTC (Coordinated Universal Time).
       CRLF           = CR LF
Each header field consists
   of a name followed by a colon (":") and the field value. Field names
   are case-insensitive. The field value MAY be preceded by any amount
   of LWS, though a single SP is preferred. Header fields can be
   extended over multiple lines by preceding each extra line with at
   least one SP or HT. Applications ought to follow "common form"

	    HTTP applications have historically allowed three different formats
   for the representation of date/time stamps:

      Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
      Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
      Sun Nov  6 08:49:37 1994       ; ANSI C's asctime() format

*/

/*
		Response      = Status-Line               ; Section 6.1
                       *(( general-header        ; Section 4.5
                        | response-header        ; Section 6.2
                        | entity-header ) CRLF)  ; Section 7.1
                       CRLF
                       [ message-body ]          ; Section 7.2

general-header = Cache-Control            ; Section 14.9
                      | Connection               ; Section 14.10
                      | Date                     ; Section 14.18
                      | Pragma                   ; Section 14.32
                      | Trailer                  ; Section 14.40
                      | Transfer-Encoding        ; Section 14.41
                      | Upgrade                  ; Section 14.42
                      | Via                      ; Section 14.45
                      | Warning                  ; Section 14.46


	Response      = Status-Line               ; Section 6.1
                       *(( general-header        ; Section 4.5
                        | response-header        ; Section 6.2
                        | entity-header ) CRLF)  ; Section 7.1
                       CRLF
                       [ message-body ]          ; Section 7.2


*/

/*
	example:
		HTTP/1.1 200 OK\r\n
		Content-Length: 55\r\n
		Content-Type: text/html\r\n
		Last-Modified: Wed, 12 Aug 1998 15:03:50 GMT\r\n
		Accept-Ranges: bytes\r\n
		ETag: “04f97692cbd1:377”\r\n
		Date: Thu, 12 Jan 2024 15:26:07 GMT\r\n
		\r\n
		<55-character response>
*/
// Response::Response(void)
// {
// 		_html_content.clear();
// 	std::cout << "Response default constructor" << std::endl;
// }

Response::Response(ConfigParser &configParser, Request &request, char *env[]): _configParser(configParser), _request(request)
{
	// to avoid error of unused argumentss
	if (_configParser.getParameterValue(0, "listen").length() != 0 || _request.get_method().compare("Hi") == 0 || env == 0)
		std::cout << "";


	_html_content_size = 0;
	_html_content.clear();
	_response.clear();


	std::cout << "Response default constructor" << std::endl;


}

/*
	 "HTTP/1.1 200 OK\r\nContent-Length: 88\r\nContent-Type: text/html\r\n\r\n
		cgi file must be chmod 755 hello.py
*/

// Response::Response(Response const &src)
// {
// 	(void)src;
// }

Response	&Response::operator=(Response const &rhs)
{
	(void)rhs;
	return (*this);
}

void	Response::_setup_response(char *env[])
{
	// std::cout << "Response parametric constructor" << std::endl;
	_html_content_size = 0;
	 _envp = env;
	_html_content.clear();
	_status_line.clear();
	_response.clear();

}

// void	Response::set_error_page_map(std::map<std::string, std::string> *error_page_map)
// {
// 	_error_page_map = error_page_map;
// }

// std::string	Response::get_response(void) const
// {
// 	return (_response);
// }

void	Response::create_response(int server_id)
{
	if (server_id == -1) // consume as it unused at the moment
		std::cout << "" << std::endl; 
	_setup_response(_envp);
	_parse_response(_request);

}

std::string		Response::get_response(void) const
{
	return (_response);
}



void Response::process_cgi(char const *buffer, int buffer_size)
{
	if (buffer_size == 0) // consume as it is not used at the moment
		std::cout << "";
	// dup2(_response.get_fd_stdin(), STDIN_FILENO); 
	// create a response
	// close(_ fd_pipe[0]);
	_create_status_line();

	// append response
	_response.append(_status_line);
	_response.append("Server: Webserv\r\n");

	_response.append(buffer);

}
void	Response::_parse_response(Request const &req)
{
	//  _response ="HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: Apache/2.2.14 (Win32)\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 88\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html><body><h1>Hello, World9</h1></body></html>";
	_is_cgi = false;
	// check if it is a cgi
	std::string uri = req.get_uri();
	if (uri.find("cgi-bin/") != std::string::npos)
	{
			_is_cgi = true;
		_fd_stdin = dup(STDIN_FILENO); // save original stdin
		_fd_stdout = dup(STDOUT_FILENO); // save original stdin

		if (pipe(_fd_pipe) == -1)
		{
			// error
			std::cout << "Error: pipe creation" << std::endl;
			return ;
		}
		// it is a cgi
		pid_t pid;
		pid = fork();
		if (pid == 0)
		{
			std::cout << "...Trying to run CGI : " << uri << std::endl;

			// std::string	python_path;
			char *python_path;

			char python_path_linux[] = "/usr/bin/python3";
			char python_path_mac[] = "/usr/local/bin/python3";

			python_path = python_path_mac;
			if (OS_PATH == LINUX)
				python_path = python_path_linux;


		
			uri.erase(0, 1);
			char *exe[3] = {python_path, (char *)uri.c_str(), 0};

			dup2(_fd_pipe[1], STDOUT_FILENO);
			close(_fd_pipe[0]);
			close(_fd_pipe[1]);
			close(_fd_stdin);

			execve(exe[0], exe, _envp);

			dup2(_fd_stdout, STDOUT_FILENO); // return output to stdout to print error
			std::cout << "Error: CGI could not be executed : " << uri << std::endl;
			std::cout << REDB << "reason:" << strerror(errno) <<  RESET << std::endl;
			exit(1);
		}
		else
		{
			close(_fd_pipe[1]);
			close (_fd_stdin);
			close(_fd_stdout);
			return ;
		}

	}

	if (_read_file_data(req) == -1)
		return ;

	// create a response
	_create_status_line();

	// // append response
	_response.append(_status_line);

	// content length
	std::stringstream	ss_len;

	ss_len << "Server: Webserv" << "\r\n";

	ss_len << "Content-Length: " << _html_content_size +1  << "\r\n";
	ss_len << "Content-Type: " << "text/html"  << "\r\n";

	_response.append(ss_len.str()); // convert string stream to a string

	_response.append("\r\n"); // empty line
	_response.append(_html_content);
	// std::cout << "file content is size " << _html_content_size << " (" << int2str(_html_content_size) << ")" << std::endl;
}


		
bool		Response::get_is_cgi(void)
{
	return (_is_cgi);
}


int	Response::get_fd_stdin(void) const
{
	return (_fd_stdin);
}


int	Response::get_fd_pipe_0(void) const
{
	return (_fd_pipe[0]);
}


Response::~Response(void)
{

	std::cout << "Response deconstructor" << std::endl;

}

#include <fcntl.h>
int	Response::_read_file_data(Request const &req)
{
	std::ifstream 		html_file;
	std::stringstream	ss;
	std::string file = ".";
	file.append(req.get_uri());
	// std::string			fcontent;

	// std::cout << "...trying to open <" <<req.get_uri() << ">" << std::endl;

	html_file.open(file.c_str(), std::ifstream::in);
	if (html_file.is_open() == 0)
	{
		std::cout << REDB << "Error: _get_file_data. File could not be openned" << RESET << std::endl;

		// at the moment the path to the style and img are not matching the current structure.
		_error_page_map["404"];
		html_file.open("./html/error404.html", std::ifstream::in);

		if (html_file.is_open() == 0)
		{
			std::cout << REDB << "Error: attempt to open 404 page. File could not be openned" << RESET << std::endl;

		}
	}

	/* read file */

	ss << html_file.rdbuf();

	/* convert to string */

	_html_content = ss.str();

	/* get msg body size */
	_html_content_size = _html_content.length();

	// html file content is
	// std::cout << "html File content is <" << _html_content << ">" << std::endl;

	/* readbuffer to output stream */
	// _html_content << html_file.rdbuf();

	html_file.close();
	return (0);
}

std::string	Response::get_html_content(void) const
{
	return (_html_content);
}

size_t	Response::get_html_size(void) const
{
	return (_html_content.length());
}


/*
	Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
*/
int		Response::_create_status_line(void)
{
	_status_line = "HTTP/1.1 200 OK\r\n";
	return (0);
}

std::map<std::string, std::string>	Response::get_error_page_map(void) const
{
	return (_error_page_map);
}

std::map<std::string, std::string>	Response::get_response_status_map(void) const
{
	return (_response_status_map);
}
