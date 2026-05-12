#define PQXX_HAVE_SOURCE_LOCATION 0
#include "manager.hpp"
#include <iomanip>


int user_manager::Get_Total_Users_For_Dev(int dev_id) {
    try {

        pqxx::connection C(conn_str());
        if (!C.is_open()) {
            std::cerr << "DB_CONNECTION_ERROR: Unable to connect to the database" <<"\n";
            return 0;
        }
        // This SQL query joins the apps and users to get a grand total
        std::string query = 
            "SELECT COUNT(u.user_id) "
            "FROM nexus_users u "
            "JOIN nexus_apps a ON u.app_id = a.app_id "
            "WHERE a.owner_id = " + std::to_string(dev_id) + ";";

        
        pqxx::work W(C);
        pqxx::result R = W.exec(query);
        
        // Return the count (first row, first column)
        return R[0][0].as<int>();
        
    } catch (const std::exception& e) {
        std::cerr << "CORE_ERROR: Failed to fetch user metrics: " << e.what() << std::endl;
        return 0; // Return 0 if the query fails
    }
}


bool user_manager::Register_New_App(const NexusApp& app) {
    try {

        pqxx::connection C(conn_str());
        if (!C.is_open()) { 
            std::cerr << "DB_CONNECTION_ERROR: Unable to connect to the database" << std::endl;
            return false;
        }
        // We explicitly define the columns to ensure 'name' doesn't end up in 'id'
        std::string query = "INSERT INTO nexus_apps (app_id,owner_id,app_name) VALUES ('"
            + app.app_id + "', "
            + "'" + std::to_string(app.owner_id) + "', "
            + "'" + app.name+ "');";
            

        // Execute using your PQXX worker
        pqxx::work W(C);
        W.exec(query);
        W.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "DB_INSERT_ERROR: " << e.what() << std::endl;
        return false;
    }
}
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
        
        std::cout << "                                                                             | " << std::left << std::setw(30) << user.username<< " | " << std::left << std::setw(30) << user.email << " |\n";
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
    auto it = std::find_if(Users.begin(),Users.end(),[&search_id](const Details &ID){return ID.email == search_id;});

    if(it != Users.end())
    {
        std::cout <<GREEN<< "[ User found ]\n"<<RESET;
        std::cout << "Name : " <<it->username <<"\n";
        std::cout << "Email: " <<it->email <<"\n";
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
