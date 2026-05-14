#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>


class config_loader
{
    public :

    static std::unordered_map<std::string, std::string> load_config(const std::string& filename);
};