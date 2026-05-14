#pragma once 
#include <string>

struct NexusApp {
    std::string app_id;     // unique MV-XXXX
    int owner_id;           // The dev_id of the creator
    std::string name;       // app_name
    std::string api_key;    // generated secret
    std::string created_at;  // timestamp of creation
    std::string environment = "sandbox"; // production/sandbox

    // Constructor ensures correct order for push_back/front
    NexusApp(int oid, std::string n, std::string aid, std::string key, std::string timestamp)
        : owner_id(oid), name(std::move(n)), app_id(std::move(aid)), 
          api_key(std::move(key)), created_at(std::move(timestamp)) {}
};