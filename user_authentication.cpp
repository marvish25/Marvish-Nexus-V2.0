#include "user_authentication.hpp"
#include <random>
#include <string>

int count = 7;
std::string user_authentication::salt()
{
    // count =7;
    std::string salt;


    std::random_device rd;

    std::mt19937 angine(rd());

    while(count > 0)
    {
        std::uniform_int_distribution<int> dist(33,126);
        int num = dist(angine);
        if(num < 33)
        {
            continue;
        }
       
        {
            char digit = static_cast<char>(num);
            
            salt.push_back(std::move(digit));
            --count;
        }
    }

    return salt;
}


Status user_authentication::Scan_Username(const std::string first_name, const std::string last_name)
{
    if (Email_validation(first_name) || Email_validation(last_name))
    {
        return ERR_INVALID_ID;
    }
    return SUCCESS;
}

Status user_authentication::Scan_ID(const std::string ID)
{
    if(!Email_validation(ID)) 
    {
        
        return ERR_INVALID_ID;
    }
    for (auto user : storage_obj.get_manager().get_Users())
    {
        if (ID == user.ID)
        {
            
            found = true;
            return ERR_USER_FOUND;
        }
        
    }
        
    return SUCCESS;
}
Status user_authentication::Sign_In( std::string firstName ,std::string lastName ,std::string userId , std::string userPassword , std::string appId)
{
    
        std::cout << "\nfN : "<< firstName << "\nLN :"<<lastName<<"\nU_ID:"<<userId<<"\nU_pw :"<< userPassword<<"\nAPPid:"<< appId<<"\n";
        for(auto& user : Modify_Storage_Obj().Modify_Manager_Obj().Modify_Users())
        {
            if(user.ID == userId)
            {
                std::cout << "user already exists\n";
                return ERR_USER_FOUND;
            }
        }
        std::string temp_salt = salt();
        temp_manager.temp_pw = userPassword;
        temp_manager.confirm_pw = temp_manager.temp_pw;
        switch(check_password(temp_manager.confirm_pw,temp_manager.temp_pw))
        {
            case 201:
                return ERR_PASS_DONT_MATCH;
            case 202:
                return ERR_WEAK_PASS;
            
        }

        if(appId.empty()){
            appId = "null_app";
        }

        temp_manager.confirm_pw = temp_manager.confirm_pw + temp_salt;
        
        std::string secure_hash = hash_func(temp_manager.confirm_pw);

        storage_obj.Modify_Manager_Obj().Modify_Users().push_front({std::move(firstName),
                                                                    std::move(lastName),
                                                                    std::move(userId),
                                                                    std::move(secure_hash),
                                                                    std::move(temp_salt),
                                                                    std::move(appId)
        });
        storage_obj.to_db();
        
    
    return SUCCESS;
}

Status user_authentication::check_password(const std::string& confirm_pw,const std::string& temp_pw)
{
    std::string strong_pw = "!@#$%&:;";
    bool pass_strength = false;
    int count = 0;
    //std::string temp_salt = salt();

        
        if (confirm_pw != temp_pw)
        {
            
            return ERR_PASS_DONT_MATCH;
        }
        else
        {
            for (char cur_letter : temp_manager.temp_pw)
            {
                for (char curr_letter : strong_pw)
                {
                    if (cur_letter == curr_letter)
                    {
                        count++;
                    }
                }
            }
            count >= 2 ? pass_strength = true : pass_strength = false;
        }
        if (pass_strength == false && temp_manager.confirm_pw == temp_manager.temp_pw)
        {
            
            return ERR_WEAK_PASS;
        }
    
    return SUCCESS;
}


Status user_authentication::LogIn(const std::string& app_id,const std::string& User_id , const std::string& User_password)
{
    
    int attempts = 4;
    std::string input_hash;
    std::string choice; // THIS IS A TEMP CHOICE TO BE REMOVED SOON
    
    std::cout << "app_id    : "<< app_id <<"\n";
    std::cout << "User ID   : "<< User_id <<"\n";
    
    
    
    for (auto user : storage_obj.get_manager().get_Users())
    {
        input_hash = hash_func(User_password + user.salt_value);

        if(user.ID == "marvish26@hotmail.com"){
            user.app_id = app_id;
        }
        
        if (User_id == user.ID && input_hash == user.password && user.app_id == app_id)
        {
            
            found = true;
            logged_in_password = input_hash;
            *logged_in_user = user;
            return SUCCESS;
        }
        else if (User_id == user.ID && input_hash != user.password && user.app_id == app_id)
        {

            //store the login attempts in a database and reset them afte 12 hours.
            return ERR_WRONG_PASS;

        }
    }
    
    return ERR_NOT_FOUND;
}


bool user_authentication::Email_validation(const std::string &email) {
    // The "Beast" - This follows the official internet standards (RFC 5322)
    // It handles special characters, subdomains, and modern TLDs
    const std::regex pattern(
        R"(^(?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:(25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])\.){3}(25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\])$)",
        std::regex_constants::icase // This makes it case-insensitive
    );
    
    return std::regex_match(email, pattern);
}


Status user_authentication::reset_password(const std::string confirm_pw , const std::string temp_password)
{

    std::string input_hash = hash_func(temp_manager.temp_pw + logged_in_user->salt_value);

    if (input_hash != logged_in_user->password)
    {
        return ERR_WRONG_PASS;
    }
    
    switch(check_password(confirm_pw,temp_password))
    {
        case 201:
            return ERR_WRONG_PASS;
        case 202:
            return ERR_WEAK_PASS;
    }
    

    logged_in_user->password = input_hash;
    
    for (auto &user : storage_obj.Modify_Manager_Obj().Modify_Users())
    {
        if (user.ID == logged_in_user->ID)
        {
            user.salt_value = salt();
            user.password = hash_func(temp_manager.confirm_pw+user.salt_value);
            break;
        }

    }
    
    

    storage_obj.to_db();
    return SUCCESS;
}

Status user_authentication::delete_account()
{
    storage_obj.Modify_Manager_Obj().remove_user(logged_in_user->ID);
    storage_obj.from_db();
    return SUCCESS;
}

void user_authentication::verify_logged_in_user()
{
    // This function can be used to verify the integrity of the logged-in user's data
    // For example, checking if the user still exists in the system or if their data has been tampered with
    while(true)
    {
        if(logged_in_user->password != logged_in_password)
        {
            std::cout << RED << "[ User verification failed ]\n" << RESET;
            return;
            // Handle verification failure (e.g., log out the user, alert admin, etc.)
        }   
    }
}




std::string user_authentication::hash_func(const std::string &plain_text)
{
    std::hash<std::string> hasher;
    size_t hashed_value = hasher(plain_text); // Converts string to a huge number
    return std::to_string(hashed_value);      // Returns that number as a string
}