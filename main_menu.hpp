#pragma once
#include "user_authentication.hpp"


class menu
{


user_authentication auth;
public:
    
    const user_authentication &get_auth() const 
    {
        return auth;
    }
    user_authentication &Modify_Auth_Obj()
    {
        return auth;
    }
    void user_menu();
    void main_menu();


   
};