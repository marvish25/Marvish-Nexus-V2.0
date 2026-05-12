#pragma once
#include "Load_and_unload_data.hpp"
#include "Email_Service.hpp"


class user_authentication
{

    
    
protected:
    
    
DatabaseManager storage_obj ;
Email_Service email;
public:
    std::unique_ptr<Details> logged_in_user = std::make_unique<Details>();
    
    std::string logged_in_password;         // i should make it private and all the functions that use it as friend functions
    user_manager temp_manager;

    const DatabaseManager &get_storage_obj() const
    {
        return storage_obj;
    }
    DatabaseManager &Modify_Storage_Obj()
    {
        return storage_obj;
    }
    bool found = false;
    bool Email_validation(const std::string &email);
    Status Sign_In(std::string firstName ,std::string lastName ,std::string userId , std::string userPassword , std::string appId);
    Status LogIn(const std::string& app_id,const std::string& User_id , const std::string& User_password);
    Status Scan_Username(const std::string first_name, const std::string last_name);
    Status Scan_ID(const std::string ID);
    Status check_password(const std::string& confirm_pw,const std::string& temp_pw);
    Status reset_password(const std::string confirm_pw, const std::string temp_pw);
    Status delete_account();
    void verify_logged_in_user();  // I will work on this function
    std::string salt();
    std::string hash_func(const std::string &password);
    user_authentication() = default;
    
};