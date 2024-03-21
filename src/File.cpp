/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   library.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgranero <mgranero@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 20:15:06 by mgranero          #+#    #+#             */
/*   Updated: 2024/02/04 20:27:47 by mgranero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"


File::File(void): _content_disposition(""), _name(""), _filename(""), _content_type("")
{

}

File::~File(void)
{

}


File::File(File const &src): _content_disposition(src.get_content_disposition()), _name(src.get_name()), _filename(src.get_filename()), _content_type(src.get_content_type())
{

}

File &File::operator=(File const &rhs)
{
    _content_disposition = rhs.get_content_disposition();
    _name=rhs.get_name();
    _filename=rhs.get_filename();
    _content_type=rhs.get_content_type();
    return (*this);
}

std::ostream & operator<<(std::ostream & o, const File &f)
{
	o << "Content-Disposition = <" <<  f.get_content_disposition() << ">" << std::endl;
	o << "name = <" <<  f.get_name() << ">" << std::endl;
	o << "filename = <" <<  f.get_filename() << ">" << std::endl;
	o << "Content-Type = <" <<  f.get_content_type() << ">" << std::endl;
	return o;
}


int         File::get_fd(void) const
{
    return (_fd);
}

std::string File::get_content_disposition(void) const
{
    return(_content_disposition);
}

std::string File::get_name(void) const
{
    return (_name);
}

std::string File::get_filename(void) const
{
    return (_filename);
}

std::string File::get_content_type(void) const
{
    return(_content_type);
}

void File::set_fd(int nb)
{
    _fd = nb;
}

void File::set_content_disposition(std::string str) 
{
   _content_disposition = str;
}

void File::set_name(std::string str) 
{
    _name = str;
}

void File::set_filename(std::string str) 
{
    _filename = str;
}

void File::set_content_type(std::string str) 
{
    _content_type = str;
}


