#pragma once
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include "manager.hpp"


class Email_Service
{
    public:
    void send_reset_email(std::string phone_number);
    void verify_and_update_db(std::string phone , std::string user_input , std::string new_password);
};