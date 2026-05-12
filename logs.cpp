#include "logs.hpp"

std::string NexusLogger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void NexusLogger::nexus_log(const std::string& filename, const std::string& msg) {
    std::ofstream log_file(filename, std::ios_base::app);
    if (log_file.is_open()) {
        log_file << "(" << get_timestamp() << ") " << msg << std::endl;
    }
}

void NexusLogger::log_nexus_activity(const crow::request& req, std::string action) {
    std::ofstream log_file("nexus_audit.log", std::ios_base::app);
    if (log_file.is_open()) {
        log_file << "[" << get_timestamp() << "] " 
                 << "IP: " << req.remote_ip_address << " | "
                 << "ACTION: " << action << std::endl;
    }
}

void NexusLogger::log_event(std::string message) {
    std::ofstream log_file("system.log", std::ios_base::app);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    log_file << "(" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << ") " << message << std::endl;
}
