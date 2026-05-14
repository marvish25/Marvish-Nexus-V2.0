#include "config_loader.hpp"


std::unordered_map<std::string, std::string> config_loader::load_config(const std::string& filename)
{
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);

    if(!file.is_open())
    {
        //std::cout << "\033[31m[error opening config file] \n\033[0m";
        
    }
    
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        config[key] = value;
    }
    return config;
}
