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
#include <pqxx/pqxx>
#include "nexus_core.hpp"





const std::string RESET   = "\033[0m";
const std::string BOLD    = "\033[1m";
const std::string RED     = "\033[31m";
const std::string CYAN    = "\033[36m";
const std::string GREY    = "\033[90m";
const std::string YELLOW  = "\033[33m";
const std::string GREEN   = "\033[32m";


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

struct Details
{
    int dev_id;             // This is the unique identifier for each user, it can be generated using a simple counter or a UUID generator.
    std::string username,email,password_hash,password_salt,role,timestamp;
    

    bool online;            // This variable is not being used yet.
};

class user_manager
{
    std::list<Details> Users; // the list of the users
    std::list<NexusApp> Apps; // the list of the apps
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

    const std::list<NexusApp> &get_Apps() const
    {
        return Apps;
    }

    std::list<NexusApp> &Modify_Apps()
    {
        return Apps;
    }   
   
    void view_users();
    void clear_console();
    void search_user(const std::string &search_id);
    

    user_manager() = default;



};