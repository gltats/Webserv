#include "ConfigParser.hpp"

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

    
    int main()
    {
        { 
            // Test 1: pass
            ConfigParser configParser;
            try {
                configParser.getConfig("configs/default.conf");
                // configParser.getConfig("configs/single");
                // Should print the configuration without errors
            } catch (const std::invalid_argument& e) {
                std::cerr << "Test 1: " << e.what() << std::endl;
            }
        }

        // Test 2: Non-pass
        {
            ConfigParser configParser;
            try {
                configParser.getConfig("test_configs/non_existent.conf");
                configParser.print(); // Should throw an exception
            } catch (const std::invalid_argument& e) {
                std::cerr << "Test 2: " << e.what() << std::endl;
            }
        }

        return 0;

    }
