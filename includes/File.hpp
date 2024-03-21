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

#ifndef FILE_HPP
#define FILE_HPP

#include <iostream>

class File
{
    private:
        int         _fd;
        std::string _content_disposition;
        std::string _name;
        std::string _filename;
        std::string _content_type;

    public:
        File(void);
        ~File(void);
        File(File const &src);
        File &operator=(File const &rhs);

        int         get_fd(void) const;
        std::string get_content_disposition(void) const;
        std::string get_name(void) const;
        std::string get_filename(void) const;
        std::string get_content_type(void) const;

        void        set_fd(int fd);
        void        set_content_disposition(std::string str);
        void        set_name(std::string str);
        void        set_filename(std::string str);
        void        set_content_type(std::string str);
};

std::ostream & operator<<(std::ostream & o, const File &f);

#endif