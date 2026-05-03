#include "main_menu.hpp"
#include "manager.hpp"

void menu::main_menu()
{
    std::string choice_str;
    Details user;


    
    do
    {
        /* code */

        std::cout << "\n________________________\n";
        std::cout << "\033[32m    MAIN MENU \033[0m\n";
        std::cout << "************************\n";
        std::cout << "1. Create new account\n";
        std::cout << "2. Log in\n";
        std::cout << "3. View users\n";
        std::cout << "4. Exit\n\n";
        std::cout << "Enter choice here :";
        std::getline(std::cin >> std::ws, choice_str);
        auth.Modify_Storage_Obj().Modify_Manager_Obj().clear_console();
        if (choice_str.length() == 1)
        {
            
            switch (choice_str[0])
            {
            case '1':
                std::cout << "ente first name :";
                std::cin >> user.first_name;
                std::cout << "enter last name :";
                std::cin >> user.last_name;
                std::cout <<auth.Scan_Username(user.first_name,user.last_name) << "\n";
                std::cout << "enter email :";
                std::cin >> user.ID;
                auth.Scan_ID(user.ID);
                std::cout << "enter password :";
                std::cin >> auth.temp_manager.temp_pw;
                std::cout << "enter confirm password :";
                std::cin >> auth.temp_manager.confirm_pw;
                auth.check_password(auth.temp_manager.temp_pw,auth.temp_manager.confirm_pw);

                if(auth.Sign_In(user) == 200)
                    std::cout << "Success\n";
                else
                    std::cout << "Failed\n";
                break;
            case '2':
                std::cout << "enter email :";
                std::cin >> auth.temp_manager.temp_id;
                std::cout << "enter password :";
                std::cin >> auth.temp_manager.temp_pw;
                std::cout <<auth.LogIn("SYSTEM_ADMIN",auth.temp_manager.temp_id,auth.temp_manager.temp_pw);

                if (auth.get_storage_obj().get_manager().get_Users().empty())
                {
                    std::cout << "\033[31m[No available users in the system!]\n \033[0m";
                }
                else if (auth.found)
                {
                    user_menu();
                    auth.found = false;
                }
                
                break;
            case '3':
                if (auth.get_storage_obj().get_manager().get_Users().empty())
                {
                    std::cout << "\033[31m[no users available in the system] \033[0m \n";
                    break;
                }
                auth.Modify_Storage_Obj().Modify_Manager_Obj().view_users();
                break;
            case '4':
                std::cout << "\033[31mTHANK YOU FOR USING MY CODE RATE IT OUT OF 10 \033[0m\n";
                break;
            default:
                std::cout << "\033[31m[invalid key]\033[0m\n";

                break;
            }
        }
        else
        {
            std::cout << "\033[33m[please enter only (1,2,3 or 4)]\033[0m\n";
        }
    } while (choice_str[0] != '4');
}

void menu::user_menu()
{
    std::string delete_id,search;
    std::string choice_str;
    if (auth.get_storage_obj().get_manager().get_Users().empty())
    {
        std::cout << "\033[33m[no users available in the system]\n\033[0m";
        auth.found = false;
        return;
    }
    

    do
    {

        std::cout << "\n\033[1m__________________________________\033[0m\n";
        std::cout << "          \033[32m ⬵WELCOME " << auth.logged_in_user->first_name << "\033[0m\n";
        std::cout << "\033[1m***********************************\033[1m\n";
        std::cout << "1. Delete Account\n";
        std::cout << "2. Reset password\n";
        std::cout << "3. Search user\n";
        std::cout << "4. Log-out\n";
        std::cout << "Enter choice here :";
        std::string temp_password;
        std::getline(std::cin >> std::ws, choice_str);
        auth.Modify_Storage_Obj().Modify_Manager_Obj().clear_console();
        if (choice_str.length() == 1)
        {
            
            switch (choice_str[0])
            {
            case '1':
                std::cout <<RED<<"NOTE : Deleting your account will remove all your data from the system permanently.\n"<<RESET;
                std::cout <<"Are you sure you want to proceed? (Y/N) :";
                char confirm;
                std::cin >> confirm;
                auth.Modify_Storage_Obj().Modify_Manager_Obj().clear_console();
                if (confirm == 'Y' || confirm == 'y')
                auth.delete_account();
                auth.found = false;
                return;
            case '2':
                std::cout << "enter password :";
                std::cin >>auth.temp_manager.temp_pw;
                std::cout << "enter new password : ";
                std::cin >>temp_password;
                std::cout << "confirm new password :";
                std::cin >>auth.temp_manager.confirm_pw;
                std::cout <<auth.reset_password(temp_password,auth.temp_manager.confirm_pw);
                break;
            case '3':
                std::cout << "Enter the user Email :";
                std::getline(std::cin >> std::ws ,search);
                auth.Modify_Storage_Obj().Modify_Manager_Obj().search_user(search); // I have to make this function use get_manager() function 
                break;
            case '4':
                auth.found = false;
                return;

            default:
                std::cout << "\033[33m[invalid option!]\033[0m\n";
                break;
            }
        }
        else
        {
            std::cout << "[please enter only (1,2,3 or 4)]\n";
        }
    } while (choice_str[0] != '4');
}


