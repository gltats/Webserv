#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <iostream>
# include <string>

class ConfigFile
{

	public:

		ConfigFile();
		ConfigFile( ConfigFile const & src );
		~ConfigFile();

		ConfigFile &		operator=( ConfigFile const & rhs );

	private:

};

std::ostream &			operator<<( std::ostream & o, ConfigFile const & i );

#endif /* ****************************************************** CONFIGFILE_H */