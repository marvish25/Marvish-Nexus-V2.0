#pragma once
#include <atomic>
#include <condition_variable>
#include <thread>
#include "manager.hpp"



class DatabaseManager 
{
    std::atomic<bool> running{true};
    std::condition_variable cv;
    std::mutex cv_mtx;
    std::thread refresh_thread;
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