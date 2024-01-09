/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:33
 * @ Modified by: Gl.tats
 * @ Modified time: 2024-01-09 17:25:41
 * @ Description: webserv
 */

#pragma once

#include <iostream>

class ConfigParser {
	private:
		std::string		_path;
		size_t			_size;
		size_t			_nb_server;
	protected:
		void getConfig(const std::string &configFile);
		void removeWhiteSpace(const std::string& content);
		void removeComments(const std::string& content);
		int getTypePath(std::string const path);
		int	checkFile(std::string const path, int mode);
		std::string	readFile(std::string path);
		bool isFileExistAndReadable(std::string const path, std::string const index);
		bool fileOpen(std::ifstream &configFile);
	public:
		ConfigParser();
		ConfigParser(std::string const ConfigFile);
		ConfigParser(const Parsing &copy);
		ConfigParser &operator=(const Parsing &copy);
		~ConfigParser();
		class BracketsNotClosed : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};
		
		class BlocknameNotExisting : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};

		class NoSemicolonAtTheEndOfContext : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};

		class FileNotOpen : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};

		class LocationAlreadyExists : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};
		
		class ContextExistsMoreThanOnce : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};
		
		class FileIsUnaccessible : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};

		class FileIsEmpty: public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};

		class FileIsNotFile : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};

		class PathIssue : public std::exception
		{
		public:
			virtual const char	*what() const throw();
		};
}