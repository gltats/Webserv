/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI_handler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankinzin <ankinzin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 14:37:39 by ankinzin          #+#    #+#             */
/*   Updated: 2024/01/25 16:07:39 by ankinzin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_WEBSERVER_HPP
#define FT_WEBSERVER_HPP

#include <string>
#include <iostream>
#include <map>
#include <cstdio>

#include <sys/wait.h> // waitpid
#include <unistd.h> // fork, execve
#include <cstring> // strerror

#define GREEN "\033[1;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"
#define SECONDS 5

class CgiHandler {
public:
	CgiHandler(const Server &server, const Request &request);

	CgiHandler(CgiHandler const &rhs);

	~CgiHandler();

	CgiHandler &operator=(CgiHandler const &rhs);

	std::string startCgiHandler(std::string &response);

	void printMessage();

	void _closeAll(int fdInput, int fdOutput, int saveStdin, int saveStdout, FILE *fileInput, FILE *fileOutput);

private:
	CgiHandler();

	Request _request;

	TypeHelper _typeHelper;

	std::map<std::string, std::string> _env;

	char **_envArray;

	char **_getEnvironmentVariables();

	void _initEnvironmentVariables();

	std::string toString(std::string::size_type i);

	bool _isPythonScript();

	std::string _handlePythonScript(std::string &response);

	std::string _handleBinaryScript(std::string &response);


};

std::ostream &operator<<(std::ostream &out, CgiHandler const &in);

#endif