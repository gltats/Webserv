#include "Response.hpp"


// std::string	Response::getLastReadTime(struct dirent *dirName)
// {
// 	struct stat st;
// 	std::string dirname = dirName->d_name;
// 	std::string tmpPath = server.root + "/" +  dirname;

// 	if (stat(tmpPath.c_str(), &st))
// 		return("Can't get the folder infos");

// 	time_t accessTime = st.st_atime;
	
// 	return(ctime(&accessTime));

// }

/* ************************************************************************** */
// std::string Response::getSize(struct dirent *dirName)
// {
// 	struct stat st;
// 	std::stringstream s;
// 	std::string dirname = dirName->d_name;
// 	std::string tmpPath = server.root + "/" +  dirname;

// 	if (stat(tmpPath.c_str(), &st))	
// 		return(0);	
// 	s << st.st_size;

//  	return(s.str());
// }

/* ************************************************************************** */


// std::string Response::dirLink(struct dirent *dirName)
// {
// 	std::stringstream s;
// 	std::string tmpPath;

// 	std::string time = getLastReadTime(dirName);
// 	std::string size = getSize(dirName);
// 	if (!_path.compare("/"))
// 		s << "<p><a href=\"http://" +  _host << _path + dirName->d_name + "\"><b><i><font size=\"5\">" \
//    		+ dirName->d_name + "/</font></i></b></a>" << "\t\t" << "<b><i><font size=\"5\">" + size + "\t\t" + time \
// 	 	+ "</font></i></b></p>\n";

// 	else
// 		s << "<p><a href=\"http://" +  _host << _path + "/" + dirName->d_name + "\"><b><i><font size=\"5\">" \
//     	+ dirName->d_name + "/</font></i></b></a>" << "\t\t" << "<b><i><font size=\"5\">" + size + "\t\t" + time \
// 	 	+ "</font></i></b></p>\n";
// 	return (s.str());
// }

/* ************************************************************************** */

std::string Response::getIndex()
{
// 	DIR *dir = opendir(server.root.c_str());
// 	std::string dirs =\
// 	 "<!DOCTYPE html>\n\
//     <html>\n\
//     <head>\n\
// 	<meta charset=\"UTF-8\">\n\
//             <title>" + server.root + "</title>\n\
//     </head>\n\
//     <body style=\"text-align:center;\">\n\
//    <h1> Index of "+ server.root +" </h1>\n\
//    <pre style=\"text-align:center;\">\n\
//    <p>" ;
	
// 	if (dir == NULL)
// 	{
// 		std::cerr << "Error: could not open  the " << server.root <<  std::endl;
//         return ""; // ^ create an error.html for this
// 	}
// 	for (struct dirent *dirName = readdir(dir); dirName != NULL; dirName = readdir(dir))
// 	{
// 		if (std::string(dirName->d_name)[0] != '.')
//         	dirs +=  dirLink(dirName);
// 	}
//     dirs += " \
//     </p>\n\
// 	</pre>\n\
//     </body>\n\
//     </html>\n";
//     closedir(dir);
//     return dirs;
	std::string string = "Hello There";

	return string;
	
	
}

/* ************************************************************************** */

int Response::deafIndex()
{ 
	if (!_server.getLocationValue(0, 0,"location").empty())
	{
		_path = _server.getLocationValue(0, 0, "location"); // ask Maira for this path
		return(200);
	}
	return(404);
}

/* ************************************************************************** */

void Response::isAutoindex(size_t i)
{
	if (i == std::numeric_limits<size_t>::max())
		_autoindex = false;
	else if (_server.getLocationValue(0,0, "autoindex") == "true")
		_autoindex = true;
	// isDefaultindex();
}
