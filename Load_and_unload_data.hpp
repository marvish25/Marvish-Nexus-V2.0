#pragma once
#include <atomic>
#include <condition_variable>
#include <thread>
#include "config_loader.hpp"
#include "manager.hpp"



class DatabaseManager 
{
    std::atomic<bool> running{true};
    std::condition_variable cv;
    std::mutex db_mtx;
    std::thread refresh_thread;
    config_loader conf;
    user_manager manager;
    public:


    DatabaseManager() {
        refresh_thread = std::thread(&DatabaseManager::reload_Users, this);
    }

    const user_manager &get_manager() const
    {
        return manager;
    }
    user_manager &Modify_Manager_Obj()
    {
        return manager;
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
    bool Execute_DB_Deletion(const std::string& app_id);
    void remove_user(const std::string& delete_id);
    int Get_Total_Users_For_Dev(int dev_id);
    bool Register_New_App(const NexusApp& app);
    void reload_Users();
    void from_db();
    void to_db();

    ~DatabaseManager() {
        running = false;
        cv.notify_all();
        if (refresh_thread.joinable()) {
            refresh_thread.join();
        }
    }
};