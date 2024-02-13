#pragma once

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
#include "../includes/config.hpp"
// #include "../includes/request.hpp"

class Response {
		private:
			std::string _index;
			std::string _root;
			std::string _conType;
			bool	_cgi;
			bool	_indexcgi;

		std::map<std::string, std::string>_header;
		std::map<std::string, std::string> _contentType;
		std::map<std::string, std::string> _postObject;
		std::map<int, std::string> _status_code;

		std::vector<std::string> cgi_header;
		std::string cgi_line;

		public:
			Response(Request &_request, fds_into &_fd);
			Response();
			~Response();

		// FUNCTIONS FOR CGI
		int					runcgi();
		int					fillClass();
		int					iscgi(std::string path);

		// FUNCTIONS FOR GET METHOD
		void 			getMethod();



		size_t 			getIndexLocation(std::string path);
		std::string 	getIndex();
		std::string 	getSize(struct dirent *dirName);

		//ERROR FUNCTIONS
		std::string 	setErrorPage(std::string path, std::string status_code);

		// HEADER GENERATING
		std::string 	get_Current_Time();
		std::string 	get_Connection();
		std::string 	get_Content_Type();
		std::string 	get_Content_Length();
		std::string 	get_Redirection();
		std::string 	get_Allow_Methods();
		std::string 	getcgiheader();
		std::string 	get_cache_control();

		std::string 	to_String(int n);

		class Cgi {
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

};