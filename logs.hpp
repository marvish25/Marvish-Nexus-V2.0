#pragma once
#include "crow_all.h"


class NexusLogger {
public:
    std::string get_timestamp();
    void nexus_log(const std::string& filename, const std::string& msg);
    void log_nexus_activity(const crow::request& req, std::string action);
    void log_event(std::string message);
};
    
