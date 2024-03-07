#include "../../../includes/config.hpp"
#include "../../../../includes/response_arafa.hpp"
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <sys/wait.h>

/*
  Description:
   This function is responsible for creating and populating the environment variables
   required for CGI execution. It takes a Response::Cgi object and a filename as parameters,
   and sets up the environment variables needed for the CGI script to access relevant information
   such as content length, content type, query string, document root, etc.

   Parameters:
   - cgi: A Response::Cgi object containing CGI-related information.
   - fileName: A string representing the filename or path of the CGI script.

   Returns:
   A dynamically allocated array of char pointers (char**) representing the environment
   variables for the CGI script. The array is terminated with a NULL pointer.

   Note: The caller is responsible for freeing the allocated memory after use.
*/

char    **setEnv(Response::Cgi cgi, std::string fileName)
{
	char **environment = (char**)calloc(13 , sizeof(char*));

	std::string variable;
	variable = "CONTENT_LENGTH=";
	variable.append(cgi.getCgicontentLength());
	environment[0] = strdup(variable.c_str());

	variable = "CONTENT_TYPE=";
	variable.append(cgi.getCgicontentType());
	environment[1] = strdup(variable.c_str());

	variable = "QUERY_STRING=";
	if (cgi.getCgiMethod() != "POST")
		variable.append(cgi.getCgiQuery());
	environment[2] = strdup(variable.c_str());

	variable = "DOCUMENT_ROOT=";
	variable.append(cgi.getCgiroot());
	environment[3] = strdup(variable.c_str());


	variable = "PATH_INFO=";
	variable.append(cgi.getCgiPath());
	environment[4] = strdup(variable.c_str());

	variable = "REQUEST_METHOD=";
	variable.append(cgi.getCgiMethod());
	environment[5] = strdup(variable.c_str());

	variable = "SERVER_PORT=";
	variable.append(cgi.getCgiPort());
	environment[6] = strdup(variable.c_str());

	variable = "SERVER_NAME=";
	variable.append(cgi.getCgiServerName());
	environment[7] = strdup(variable.c_str());

	variable = "REDIRECT_STATUS=";
	variable.append(cgi.getCgiredirectStatus());
	environment[8] = strdup(variable.c_str());


	variable = "SERVER_PROTOCOL=";
	variable.append(cgi.getCgiserverProtocol());
	environment[9] = strdup(variable.c_str());


	variable = "HTTP_COOKIE=";
	variable.append(cgi.getCookies());
	environment[10] = strdup(variable.c_str());

	variable = "SCRIPT_FILENAME=";
	variable.append(fileName);
	environment[11] = strdup(variable.c_str());

	environment[12] = NULL;

	return environment;
}

/*

   Description:
   This function executes a CGI script given its filename and a Response::Cgi object
   containing CGI-related information. It sets up the necessary environment variables,
   creates a child process using fork(), and executes the CGI script using execve().

   Parameters:
   - fileName: A string representing the filename or path of the CGI script.
   - cgi: A Response::Cgi object containing CGI-related information.

   Returns:
   A string containing the output generated by the CGI script. The output is captured
   by redirecting the standard output to a temporary file. If any error occurs during
   execution, an exception is thrown.

   Note: The caller should handle exceptions appropriately.
*/

std::string    Response::executeCgi(std::string fileName, Cgi cgi)
{

	fileName = cgi.getCgiroot() + fileName;
	std::ifstream check;
	check.open(fileName);

	if (!check.is_open())
	{
		remove("./tmpFile");
		throw std::invalid_argument("missing lines");
	}

	// I need server locations here !!!!!!!!!!
	// std::string cmd = server.locations..................;

	remove("./tmpFile");

	int fd = open("./tmpFile", O_CREAT | O_WRONLY | O_TRUNC);
	int pid = fork();

	if (pid == 0)
	{
		int fds[2];
		pipe(fds);

		dup2(fd, 1);
		close(fd);

		if (cgi.getCgiMethod() == "POST")
		{
			write(fds[1], cgi.getCgiQuery().c_str(), cgi.getCgiQuery().length());
			dup2(fds[0], 0);
		}
		close(fds[0]);
		close(fds[1]);
		char *argv[3];
		argv[0] = strdup((char*)cmd.c_str());
		argv[1] = strdup((char *)fileName.c_str());
		argv[2] = NULL;

		char **envp = setEnv(cgi, fileName);
		execve(argv[0], argv, envp);
		exit(0);
	}

	waitpid(-1, NULL, 0);
	close(fd);


	std::string tmp;
	std::string lines;
	std::vector<std::string> alllines;

	std::ifstream test;

	test.open("./tmpFile");


	while(getline(test, tmp))
	{
		alllines.push_back(tmp + "\n");
	}

	cgi_line = "";
	if (alllines.size() > 2)
	{
		std::vector<std::string>::iterator iter = alllines.begin();

		while (iter != alllines.end())
		{
			if ( (*iter)[0] == 13 || (*(iter +1))[0] == 0 || (*(iter))[0] == '\n')
			{
				if (alllines.size() > 1)
					iter = alllines.erase(iter);
				if (alllines.size() > 1)
					iter = alllines.erase(iter);
				break ;
			}
			cgi_line += *iter;

			iter = alllines.erase(iter);
		}
		for (std::vector<std::string>::iterator it = alllines.begin(); it != alllines.end(); it++)
		{
			lines.append(*it);
		}
	}
	else
	{
		remove("./tmpFile");
		throw std::invalid_argument("missing lines");
	}

	remove("./tmpFile");
	return lines;
}