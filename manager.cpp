#define PQXX_HAVE_SOURCE_LOCATION 0
#include <pqxx/pqxx>
#include "manager.hpp"
#include <iomanip>


void user_manager::remove_user(const std::string& delete_id)
{
    std::lock_guard<std::mutex> lock(mtx);
    try {
        pqxx::connection C(conn_str());
        if (C.is_open())
        {
            //printf("Opened database succesfully : %s",C.dbname());

        }

        pqxx::work W(C);
        pqxx::result R = W.exec_params("DELETE FROM accounts WHERE email=$1",delete_id);
        W.commit();
        if(R.affected_rows() == 0)
        {
            printf("[ User not found ]\n");
            return ;
        }
        std::cout << GREEN <<" [ "<< delete_id << " has been deleted from the system ] \n"<<RESET;
        


    } catch (const std::exception &e) {
            std::cerr<< e.what() << std::endl;
        }

}

void user_manager::view_users()
{
    std::lock_guard<std::mutex> lock(mtx);
    
    std::cout << "                                                                                                  \033[1m\033[1m\033[33m      List of all Users\n"<<RESET;
    std::cout << "                                                                                              ********************************\n";
    std::cout << BOLD<<"                                                                            \033[32m [ Total users in the system: " << Users.size() << " ]\n\n\n"<<RESET;
    printf("                                                                             | %-30s | %-30s |\n", "Username", "Email");
    std::cout << "                                                                             *******************************************************************\n";
    size = 0;

    for (auto const &user : Users)
    {
        size++;
        
        std::cout << "                                                                             | " << std::left << std::setw(30) << user.first_name+" "+user.last_name<< " | " << std::left << std::setw(30) << user.ID << " |\n";
        std::cout << "                                                                             | " << std::setw(30) << " "<< " | " << std::setw(30) <<" "<<" |\n";
    }
    std::cout << "                                                                             *******************************************************************\n";
    std::cout << BOLD<<"                                                                            \033[32m [Press enter to return to the main menu ]\n"<<RESET;
    std::cin.get();
    clear_console();
}

void user_manager::search_user(const std::string &search_id)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::find_if(Users.begin(),Users.end(),[&search_id](const Details &ID){return ID.ID == search_id;});

    if(it != Users.end())
    {
        std::cout <<GREEN<< "[ User found ]\n"<<RESET;
        std::cout << "Name : " <<it->first_name <<"\n";
        std::cout << "Email: " <<it->ID <<"\n";
    }
    else{
        std::cout <<RED<<" [ User not found ! ]\n"<<RESET;
    }
    std::cout << "___________________________________\n";
    std::cout <<BOLD<<GREEN<<"[ Press enter to return to the main menu ]\n"<<RESET;
    std::cin.get();
    clear_console();
}
void user_manager::clear_console()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
