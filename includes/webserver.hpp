#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <map>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include "color_code.hpp"

#include "ConfigParser.hpp"

#include "Connection.hpp"
#include "Webserv_Exceptions.hpp"
#include "library.hpp"

#include "AServer.hpp"

# ifndef VERBOSE
#  define VERBOSE 1
# endif


# ifdef __linux__
    #include "Server_OS__linux.hpp"


# else
    # include "Server_OS__singleClient.hpp" 
# endif


void	ft_signal_handler(int signum);
void	ft_setup_sighandler(void);

#endif