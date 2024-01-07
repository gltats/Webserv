/**
 * @ Author: Gl.tats
 * @ Create Time: 2023-12-21 16:17:33
 * @ Modified by: Gl.tats
 * @ Modified time: 2024-01-03 16:34:29
 * @ Description: webserv
 */

#pragma once

#include <iostream>

class ConfigParser {
	private:
		int _nbServer;
	protected:
	public:
		ConfigParser();
		ConfigParser();
		ConfigParser(const Parsing &copy);
		ConfigParser &operator=(const Parsing &copy);
		~ConfigParser();
}