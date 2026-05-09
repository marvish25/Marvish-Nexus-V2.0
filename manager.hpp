#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <list>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include "config_loader.hpp"



const std::string RESET   = "\033[0m";
const std::string BOLD    = "\033[1m";
const std::string RED     = "\033[31m";
const std::string CYAN    = "\033[36m";
const std::string GREY    = "\033[90m";
const std::string YELLOW  = "\033[33m";
const std::string GREEN   = "\033[32m";
struct Details
{
    std::string first_name,last_name,ID, password, salt_value , app_id,role;
    bool online;            // This variable is not being used yet.
};

class user_manager
{
    
    config_loader conf;
    std::list<Details> Users; // the list of the users
public:
    
    std::string temp_pw,temp_id, temp_pn,temp_salt,temp_pn1, confirm_pw;
    int size = 0;   
    std::mutex mtx;

    const std::list<Details> &get_Users() const
    {
        return Users;
    }
    std::list<Details> &Modify_Users()
    {
        return Users;
    }

    const config_loader &get_config() const
    {
        return conf;
    }
    std::string conn_str()
    {
        auto config = get_config().load_config("config.env");
        
        return
            "dbname=" + config["DB_NAME"] +
            " user=" + config["DB_USER"] +
            " password=" + config["DB_PASSWORD"] +
            " hostaddr="  + config["DB_HOST"] +
            " port=" + config["DB_PORT"];
            
    }

    void view_users();
    void clear_console();
    void search_user(const std::string &search_id);
    void remove_user(const std::string& delete_id);

    user_manager() = default;



};