/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver_configuration.hpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 09:42:39 by mgranero          #+#    #+#             */
/*   Updated: 2024/04/13 09:57:42 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_CONFIGURATIONS
# define WEBSERVER_CONFIGURATIONS

# include "color_code.hpp"

// Set to 1 to print more logging informations
# ifndef VERBOSE
#  define VERBOSE 1
# endif

// socket configuraiton
# ifndef MAX_BACKLOG_QUEUE
#  define MAX_BACKLOG_QUEUE 128
# endif

// epoll loop
# ifndef MAX_EVENTS
#  define MAX_EVENTS 10
# endif

// read file descriptor
# ifndef MAX_MSG_BUFFER
#  define MAX_MSG_BUFFER 8192*200
# endif

// Request
# ifndef MAX_REQUEST_LINE_LEN
#  define MAX_REQUEST_LINE_LEN 8000
# endif

# ifndef MAX_FIELD_NAME_LEN
#  define MAX_FIELD_NAME_LEN 256
# endif

# ifndef MAX_FIELD_VALUE_LEN
#  define MAX_FIELD_VALUE_LEN 512
# endif

# ifndef MAX_BODY_SIZE
# define MAX_BODY_SIZE 200
# endif

// set it to -1 to deactivate keep alive persistent connections TODO is this being used?
# ifndef KEEP_ALIVE_TIME_SECONDS
# define KEEP_ALIVE_TIME_SECONDS 30
# endif

// Response
# ifndef ROOT
#define ROOT "./frontEnd" // TODO include in Response.cpp the include to webserver_configuration
# endif

#endif