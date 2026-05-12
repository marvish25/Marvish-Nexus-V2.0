#pragma once 
#include <string>

struct NexusApp {
    int owner_id;           // The dev_id of the creator
    std::string name;       // app_name
    std::string app_id;     // unique MV-XXXX
    std::string api_key;    // generated secret
    std::string environment; // production/sandbox

    // Constructor ensures correct order for push_back/front
    NexusApp(int oid, std::string n, std::string aid, std::string key, std::string env)
        : owner_id(oid), name(std::move(n)), app_id(std::move(aid)), 
          api_key(std::move(key)), environment(std::move(env)) {}
};