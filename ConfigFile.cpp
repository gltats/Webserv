#include "ConfigFile.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ConfigFile::ConfigFile()
{
}

ConfigFile::ConfigFile( const ConfigFile & src )
{
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ConfigFile::~ConfigFile()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ConfigFile &				ConfigFile::operator=( ConfigFile const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, ConfigFile const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */