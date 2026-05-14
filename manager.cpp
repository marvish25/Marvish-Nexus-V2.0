#define PQXX_HAVE_SOURCE_LOCATION 0
#include "manager.hpp"



void user_manager::view_users()
{
    std::lock_guard<std::mutex> lock(mtx);
    

}

void user_manager::search_user(const std::string &search_id)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::find_if(Users.begin(),Users.end(),[&search_id](const Details &ID){return ID.email == search_id;});

    if(it != Users.end())
    {
        std::cout <<GREEN<< "[ User found ]\n"<<RESET;
    }
    else{
        std::cout <<RED<<" [ User not found ! ]\n"<<RESET;
    }
    
}
void user_manager::clear_console()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
