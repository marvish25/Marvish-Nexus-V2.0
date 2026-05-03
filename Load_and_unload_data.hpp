#pragma once
#include "manager.hpp"



class Data 
{
    
    user_manager manager;
    public:

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


};