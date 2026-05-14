#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <mutex>
#include "user_authentication.hpp"
#include "Email_Service.hpp"
#include <curl/curl.h>
#include "logs.hpp"


struct CorsMiddleware
{
    struct context {};



    void before_handle(crow::request& , crow::response& , context& ) {}
    
    void after_handle(crow::request& , crow::response& res, context&) {
        res.set_header("Access-Control-Allow-Origin", "*"); 
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.set_header("Connection", "Keep-Alive");

        
    }
    
};

class routes {

    NexusLogger log;
public:


    // Pass the app by reference so we can attach routes to it
    void setup_routes(crow::App<CorsMiddleware> app);
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);
};

