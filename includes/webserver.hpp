#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP


#include <map>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include "color_code.hpp"
// #include "Request.hpp"
// #include "Response.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"
#include "Connection.hpp"
#include "Webserv_Exceptions.hpp"
#include "lib_convertion.hpp"

# ifndef OS_SYS
# define OS_SYS
	enum{
		LINUX,
		MAC
	};

# if defined (__linux__)
		#define OS LINUX
#endif
# if defined (__APPLE__)
	#define OS MAC
#endif

#endif

# if defined (__APPLE__)
	#define OS MAC
	#include	<sys/event.h> // for kqueue
#endif

// # elif defined (__linux__)

void	ft_signal_handler(int signum);
void	ft_setup_sighandler(void);

#endif