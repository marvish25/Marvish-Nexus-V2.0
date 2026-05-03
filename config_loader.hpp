#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>

enum Status {
    SUCCESS = 200,
    ERR_NOT_FOUND = 404,
    ERR_WRONG_PASS = 401,
    ERR_DB_DOWN = 500,
    ERR_PASS_DONT_MATCH = 201,
    ERR_WEAK_PASS = 202,
    ERR_USER_FOUND = 203,
    ERR_INVALID_ID = 204,
    ERR_CONFIG_MISSING = 501
};
class config_loader
{
    public :

    static std::unordered_map<std::string, std::string> load_config(const std::string& filename);
};