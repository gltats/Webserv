
// MOCK Config Parser
#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <map>
#include <string>

class ConfigParser
{
    private:
        std::map<int, std::map<std::string, std::string> >   _server_map;
        int                                                  _nb_servers;
    public:
        ConfigParser(void);
        ~ConfigParser(void);

        std::string get_server_map_key(int server_id, std::string);

        int         get_nb_of_servers(void) const;

        std::string get_listen(int server_id) const;

        std::string get_server_name(int server_id) const;

        std::string get_error_page404(int server_id) const;

        std::string get_allow_GET(int server_id) const;

        std::string get_allow_POST(int server_id) const;

        std::string get_allow_DELETE(int server_id) const;

        std::string get_allow_autoindex(int server_id) const;

        std::string get_allow_index(int server_id) const;

        std::string get_allow_cgi(int server_id) const;

        std::string get_default_server(int server_id) const; // new! y or n


};

#endif
