/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 21:37:17 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 11:29:32 by mgranero         ###   ########.fr       */
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

#define ROOT "./frontEnd"


#include <iostream>
#include "fcntl.h"
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <exception>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <stdlib.h>
#include <limits>
#include <iterator>
#include  <algorithm>
#include <sstream>
#include "Request.hpp"
#include "library.hpp"
#include "ConfigParser.hpp"





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

		ConfigParser 						&_server;
		Request								&_request;

		// int									_status;
		std::string							_html_content;
		size_t								_html_content_size;
		std::string							_status_line;
		// std::string							_response;

		char 								**_envp;
		std::map<std::string, std::string>	_error_page_map;
		std::map<std::string, std::string>	_response_status_map;

		int 								_fd_stdin;
		int 								_fd_stdout;
		int									_fd_pipe[2];

		int									_location_index;

		bool								_is_cgi;

		// Response(Response const &src);
		Response							&operator=(Response const &rhs);
		int									_read_file_data(Request const &req);
		int									_create_status_line(void);
		void 								_setup_response(void);
		//void								_parse_response(Request const &req);
		// added files
		std::string _response;
	std::string _path;
	std::string _pathMAtch;
	std::string _pathExtra;
	std::string	_version;
	std::string _query;
	std::string _host;
	std::string _method;
	std::string _type;
	std::string _Reqbody;
	std::string	_Resbody;
	std::string _statusline;
	std::string _bounadry;
	std::string _postType;
	std::string  conType;
	std::string _root;
	std::string _index;
	int 		_ret;
	int			_redret;
	int			_port;
	bool        _autoindex;
	bool		_cgi;
	bool		_indexcgi;
	int         _serverID;
	size_t		_l; // stands for location


	// Request &request;
	//fds_info &fd;
	//ConfigParser &server;


	std::map<std::string, std::string>_header;
	std::map<std::string, std::string> _contentType;
	std::map<std::string, std::string> _postObject;
	std::map<int, std::string> _status_code;

	std::vector<std::string> cgi_header;
	std::string cgi_line;


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


//^ Datainit functions
	void				initErrorMap();
	void				initRespMaps();
	void				initContentMap();
	void				setBool();
	void				setPath();
	void				setVersion();
	void				setQuery();
	void				setRet();
	void				setMethod();
	int					isAbsoluteURI();
	void				changeHost();
	void				setHost();
	void				setPort();
	void				setBody();
	std::string			decodePath();
	std::string			get_key(std::string line);
	std::string			get_value(std::string line);


	int					ParsingResponse();
	size_t 				getLocation();
	size_t				regularExpretion();
	size_t				exactLocation();
	size_t				prefixLocation();
	void				nextpath(std::string &path);
	size_t				rootLocation();
	int 				redirection();
	void				changeRoot();
	int					deafIndex();
	std::string 		get_index();
	int					allowed();
	int					checkPath();


	std::string			call();

	//! FUNCTIONS FOR CGI
	int					runcgi();
	int					fillClass();
	int					iscgi(std::string path);


	//! FUNCTONS FOR GET METHOD
	void				getMethod();
	void 				isAutoindex(size_t i);
	void				isDefaultindex();
	int 				isFile();
	int					readObject();
	int					readFile();
	int					isIndex();
	int					readDefault();
	size_t				getIndexLocation(std::string path);
	std::string			getIndex();
	std::string			dirLink(struct dirent *dirname);
	std::string			getLastReadTime(struct dirent *dirName);
	std::string			getSize(struct dirent *dirName);

	std::string returned; // Review this

	//! FUNCTIONS FOR POST METHOD
	void				postMethod();
	void				tokenizing_Body();
	int					postObject();
	int					upload_On();
	int					upload_Store();
	std::string			get_Boundary();
	std::string			form_Name();
	std::string			getPostType();
	std::string			getKey(std::string value);
	void				get_Data();
	void				form_Data();
	void				regularFile();
	int					valid_Type();
	int					creat();
	std::string			postAnswer();


	//! FUNCTIONS FOR DELETE METHOD
	void				deleteMethod();
	int					deleteObject();
	std::string			deleteAnswer();



	void				headerGen();
	void				headMap();

	// ^ ERROR functions
	void 				error();
	void				defError();
	void				spError(std::string path);
	std::string			setErrorPage(std::string path, std::string status_code);

	// ^ HEADER GENERATING
	std::string			get_Current_Time();
	std::string			get_Connection();
	std::string			get_Content_Type();
	std::string			get_Content_Length();
	std::string			get_Redirection();
	std::string			get_Allow_Methods();
	std::string			getcgiheader();
	std::string			get_cache_control();


	std::string			to_String(int n);

	class Cgi
	{
	private:
		std::string Method;
		std::string Path;
		std::string Query;
		std::string root;
		std::string redirectStatus;
		std::string contentLength;
		std::string contentType;
		std::string serverName;
		std::string serverProtocol;
				std::string port;
				std::string Cookies;
				bool		_cgiIndex;
	public:
		// void		setCgiserverProtocol(std::string protocol);
		void		setCgicontentType(std::string port);
		void		setCgicontentLength(std::string length);
		void		setCgiPath(Response &response);
		void		setCgiQuery(std::string query);
		void		setCgiredirectStatus(int redirect);
		void		setCgiroot(std::string root);
		void		setCgiServerName(std::string sserverName);
		void		setCgiMethod(std::string method);
		void		setCgiserverProtocol();
		void		setCgiPort(int port);
		void		setCookies(std::string cookies);
		void		checkIndex(Response &response) {

			// Response *response = &(this->response);
			if (response.iscgi(response.get_index()))
			{
				_cgiIndex = true;
			}
			else
				_cgiIndex = false;
		}

		std::string	getIndex(Response &response) {
			return (response.get_index());
		}


		std::string		getCgicontentType();
		std::string		getCgicontentLength();
		std::string		getCgiPath();
		std::string		getCgiQuery();
		std::string		getCgiredirectStatus();
		std::string		getCgiroot();
		std::string		getCgiServerName();
		std::string		getCgiMethod();
		std::string		getCgiserverProtocol();
		std::string		getCgiPort();
		std::string		getCookies();

		Cgi();
		~Cgi();
	};

	std::string     executeCgi(std::string fileName, Cgi cgi);
	// std::string		executeCgiPy(std::string fileName, Response::Cgi cgi);
	// std::string		executeCgiPhp(std::string fileName, Response::Cgi cgi);


	int				get_ret() const;

};

#endif