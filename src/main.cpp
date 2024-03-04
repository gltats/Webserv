#include "ConfigParser.hpp"

//the proper main
// int main(int argc, char **argv)
// {
//     if(argc == 1 || argc == 2)
//     {
//         std::string		config;
//         ConfigParser configParser;

//         config = (argc == 1 ? "configs/default.conf" : argv[1]);
//         configParser.getConfig(config);
// 		configParser.print(); // for testing
//     }
//     else
//         std::cout << "Wrong arguments" << std::endl;
// }


//tests function

void test()
{
    { 
        // Test 1: pass
        ConfigParser configParser;
        try {
            configParser.getConfig("configs/default.conf");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Test 1: " << e.what() << std::endl;
        } catch (const std::runtime_error& e) {
            std::cerr << "Test 1: " << e.what() << std::endl;
        }
    }
    { 
        // Test 2: non-existent file
        ConfigParser configParser;
        try {
            configParser.getConfig("configs/no_file.conf");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Test 2: " << e.what() << std::endl;
        }
    }
    { 
        // Test 3: wrong extension
        ConfigParser configParser;
        try {
            configParser.getConfig("configs/lol");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Test 3: " << e.what() << std::endl;
        }
    }
    { 
        // Test 4: file without server block
        ConfigParser configParser;
        try {
            configParser.getConfig("configs/single.conf");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Test 4: " << e.what() << std::endl;
        }
    }
}


int main()
{
    test();
    return 0;

}

 