#include "routes.hpp"

std::mutex mtx; // Global mutex for thread safety in routes

size_t routes::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char *)contents, newLength);
    }
    catch (std::bad_alloc &e)
    {
        return 0;
    }
    return newLength;
}

void routes::setup_routes(crow::App<CorsMiddleware> app)
{

    user_authentication system_interface;
    std::thread refresh_db(&DatabaseManager::reload_Users, &system_interface.Modify_Storage_Obj());

    crow::mustache::set_base("templates");

    // 2. Fix the Static Route to look inside that same templates folder
    CROW_ROUTE(app, "/static/<string>")([](std::string filename)
                                        {
        crow::response res;
        // No need for /home/marvish/... just use the local folder
        res.set_static_file_info("templates/" + filename);
        return res; });

    CROW_ROUTE(app, "/")([]
                         {
        auto page = crow::mustache::load("login.html");
        // In older Crow versions, .render() returns a struct with a .body string
        return crow::response(page.render()); });

    CROW_ROUTE(app, "/register.html")([]
                                      {
        std::lock_guard<std::mutex> lock(mtx);
        auto page = crow::mustache::load("register.html");
        return crow::response(page.render()); });

    CROW_ROUTE(app, "/index.html")([]
                                   {
        std::lock_guard<std::mutex> lock(mtx);
        auto page = crow::mustache::load("index.html");
        return crow::response(page.render()); });

    CROW_ROUTE(app, "/favicon.ico")([]()
    {
        std::lock_guard<std::mutex> lock(mtx);
        crow::response res;
        res.set_header("Content-Type", "image/svg+xml");
        res.body = R"(<svg width="32" height="32" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
                    <path d="M 35,75 L 75,35 C 90,20 90,80 75,65 L 35,25" stroke="#00ff41" stroke-width="13" stroke-linecap="round" fill="none"/>
                    <path d="M 65,25 L 25,65 C 10,80 10,20 25,35 L 65,75" stroke="#ffffff" stroke-width="13" stroke-linecap="round" fill="none"/>
                    <path d="M 38,72 L 62,48" stroke="#00ff41" stroke-width="13" stroke-linecap="butt" fill="none"/>
                </svg>)";
        return res; });

    CROW_ROUTE(app, "/Developer.html")([]
                                       {
        std::lock_guard<std::mutex> lock(mtx);
        auto page = crow::mustache::load("Developer.html");
        return crow::response(page.render()); });

    // --- AI NEURAL INTERFACE (Ollama) ---
    CROW_ROUTE(app, "/api/v1/ai/interrogate").methods("POST"_method)([](const crow::request &req)
                                                                     {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        auto body = crow::json::load(req.body);
        if (!body || !body.has("query")) return crow::response(400, "Incomplete Request");
                                                                        
        std::string user_query = body["query"].s();
                                                                        
        // 1. Remove the '-m 5' and use 127.0.0.1 to avoid DNS issues
        // 2. We use 'single quotes' for the outer shell command to avoid escaping nightmares
        // Instead of just passing user_query, wrap it in a strict instruction
        std::string system_instruction = "<|system|>Instruction: Your name is MARVISH AI the MARVISH_NEXUS kernel assistant Be brief. One sentence only. Do not market the system. Do not explain your role. Response must be under 20 words.</s><|user|>Why are you showing instructions?</s><|assistant|>";

        std::string formatted_query = system_instruction + user_query;

        std::string cmd = "curl -s http://127.0.0.1:11434/api/generate -d \"{\\\"model\\\": \\\"tinyllama\\\", \\\"prompt\\\": \\\"" + formatted_query + "\\\", \\\"stream\\\": false}\"";
                                                                        
                                                                        
        std::string result = "";
        char buffer[128];
        FILE* pipe = popen(cmd.c_str(), "r");
                                                                        
        if (pipe) {
            while (fgets(buffer, sizeof(buffer), pipe) != NULL) result += buffer;
            pclose(pipe);
        }

        // DEBUG: Print result to your terminal so you can see what Ollama is actually saying
        std::cout << "Ollama Raw Output: " << result << std::endl;

        if (result.empty()) {
            return crow::response(200, "Neural Link Offline: Check if 'ollama serve' is running in another terminal.");
        }

        auto ai_json = crow::json::load(result);
        if (!ai_json || !ai_json.has("response")) {
            return crow::response(200, "Neural Link Error: Ollama sent invalid data.");
        }

        return crow::response(200, ai_json["response"].s()); });

    // --- THE NEW APP-ID FILTERING ROUTE ---
    CROW_ROUTE(app, "/api/v1/users/<string>")([&](const crow::request &req, std::string target_app_id)
                                              { 
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        log.log_nexus_activity(req, "FILTER_SCAN_ON_" + target_app_id);
        std::vector<crow::json::wvalue> filtered_list;
        auto &users = system_interface.get_storage_obj().get_manager().get_Users();
        for (const auto &u : users) {
            
                crow::json::wvalue node;
                node["id"] = u.email;
                node["access_level"] = u.role;
                filtered_list.push_back(std::move(node));
            
        }
        crow::json::wvalue x;
        x["users"] = std::move(filtered_list);
        return x; });
    CROW_ROUTE(app, "/api/v1/apps/<string>").methods("DELETE"_method)([&system_interface](const crow::request &req, std::string app_id)
                                                                      {
        std::lock_guard<std::mutex> lock(mtx);
    
        // 1. Log the activity for the audit trail
        std::cout << "[SYSTEM] INITIATING_DELETION: " << app_id << std::endl;

        // 2. Only execute the DB deletion
        // Since you reload every 3 seconds, we don't touch the vector here.
        bool success = system_interface.Modify_Storage_Obj().Execute_DB_Deletion(app_id);

        if (success) {
            // Return plain text to stay consistent with your Login/Register patterns
            return crow::response(200, "DELETION_SUCCESSFUL");
        } else {
            return crow::response(500, "DATABASE_REJECTION: Check Kernel Logs");
        } 
    });   

    CROW_ROUTE(app, "/api/v1/system/audit-stream")([]()
    {
            std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        // 1. Open with 'in' mode explicitly
        std::ifstream file("nexus_audit.log", std::ios::in);
        std::vector<std::string> log_lines;
        std::string line;

        if (!file.is_open()) {
            // If file doesn't exist, return a message so the UI knows
            crow::json::wvalue x;
            x["logs"] = std::vector<std::string>{"[SYSTEM] Initializing audit log..."};
            return x;
        }

        // 2. Read lines (Your logic for last 20 lines is good)
        while (std::getline(file, line)) {
            if(!line.empty()) {
                log_lines.push_back(line);
            }
            if (log_lines.size() > 20) {
                log_lines.erase(log_lines.begin());
            }
        }   
        file.close();

        crow::json::wvalue x;
        x["logs"] = std::move(log_lines);
        return x; });

  CROW_ROUTE(app, "/api/v1/login").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
([&](const crow::request& req) {
    crow::response res;

    // 1. FORCE THE HEADERS - This stops the browser from blocking you
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res.set_header("Content-Type", "application/json");

    // 2. Handle OPTIONS (Pre-flight) - Browsers send this FIRST
    if (req.method == crow::HTTPMethod::OPTIONS) {
        res.code = 200;
        res.body = "{\"status\":\"OK\"}"; // Body > 0 bytes kills the 204 error
        return res;
    }

    // 3. Handle POST
    auto body = crow::json::load(req.body);
    if (!body) {
        res.code = 400;
        res.body = "{\"error\":\"INVALID_JSON\"}";
        return res;
    }

    // Safe string extraction without the ?: operator to avoid type errors
    std::string app_id = "";
    std::string id = "";
    std::string pw = "";

    if (body.has("app_id")) app_id = body["app_id"].s();
    if (body.has("id"))     id     = body["id"].s();
    if (body.has("pw"))     pw     = body["pw"].s();

    // Your Kernel Logic
    Status result = system_interface.LogIn(app_id, id, pw);

    if (result == SUCCESS) {
        res.code = 200;
        crow::json::wvalue x;
        x["status"] = "AUTHORIZED";
        x["token"] = "NEXUS_CORE_HANDSHAKE_SUCCESS"; 
        res.body = x.dump(); // This ensures the body length is > 0
    } else {
        res.code = 401;
        res.body = "{\"status\":\"DENIED\"}";
    }

    return res;
});
    CROW_ROUTE(app, "/api/v1/register").methods("POST"_method)([&system_interface](const crow::request &req)
    {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        
        auto x = crow::json::load(req.body);
        
        if (!x) {
            return crow::response(400, "FATAL: INVALID_BUFFER_PAYLOAD");
        }

        try {
            // Extracting from your register.html JSON keys

            std::string username = x["username"].s();
            std::string email = x["email"].s();
            std::string password = x["password"].s();
            std::string confirmPw = x["confirm_password"].s();
           

            // Interfacing with the Marvish Nexus Kernel
            Status result = system_interface.Sign_In(username, email, password, confirmPw); 

            if (result == Status::SUCCESS) {
                crow::json::wvalue res;
                res["status"] = "STABILIZED";
                res["msg"] = "Operator Profile Committed to Nexus.";
                return crow::response(200, res);
            } else if(result == Status::ERR_USER_FOUND) {
                return crow::response(203, "User already exists.");
            } else if (result == Status::ERR_PASS_DONT_MATCH) {
                return crow::response(201, "Password and Confirm Password do not match.");
             }
            else if (result == Status::ERR_WEAK_PASS) {
                return crow::response(202, "Weak Password : Must be at least 8 characters, include uppercase, lowercase, and a number.");
            }
        } catch (const std::exception& e) {
            return crow::response(500, "KERNEL_PANIC: Data Extraction Failed.");
        } 
        return crow::response(500, "UNKNOWN_ERROR: An unknown error occurred during registration."); });

    CROW_ROUTE(app, "/api/v1/users")([&]()
    {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        auto &users = system_interface.get_storage_obj().get_manager().get_Users();
        std::vector<crow::json::wvalue> user_list;
        for (const auto &u : users) {
            crow::json::wvalue node;
            node["id"] = u.email;
            node["access_level"] = u.role;
            user_list.push_back(std::move(node));
        }
        crow::json::wvalue x;
        x["users"] = std::move(user_list);
        return x; });

    CROW_ROUTE(app, "/api/v1/system/logs")([]()
                                           {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        std::ifstream file("system.log");
        std::vector<std::string> log_lines;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) log_lines.push_back(line);
            if (log_lines.size() > 15) log_lines.erase(log_lines.begin());
        }
        crow::json::wvalue x;
        x["logs"] = std::move(log_lines);
        return x; });

    CROW_ROUTE(app, "/api/v1/network/scan")([]()
                                            {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        
        std::string cmd = "sudo /usr/sbin/arp-scan --localnet";
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return crow::response(500, "Scan Failed");
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) result += buffer;
        pclose(pipe);
        return crow::response(200, result); });

    CROW_ROUTE(app, "/api/v1/deploy").methods("POST"_method)([&system_interface](const crow::request &req)
    {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        auto x = crow::json::load(req.body);
        
        // Safety check to prevent Kernel Panic
        if (!x.has("app_name") || !x.has("app_id")) return crow::response(400);

        // 1. Extract
        std::string name = x["app_name"].s();
        std::string id   = x["app_id"].s();
        std::string env  = x["env"].s();
        std::string key  = "SK_LIVE_" + id; // You can generate this randomly

        // 2. Get the logged-in dev's ID (this should come from your login session)
        int current_dev = system_interface.logged_in_user->dev_id;

        // 3. Create Object and Store
        NexusApp new_deployment(current_dev, name, id, key, env);
    
        if (system_interface.Modify_Storage_Obj().Register_New_App(new_deployment)) {
            return crow::response(200, "APP_DEPLOYED_SUCCESSFULLY");
        }
    
        return crow::response(500, "DEPLOYMENT_REJECTED_BY_DATABASE"); });

    CROW_ROUTE(app, "/api/v1/apps").methods(crow::HTTPMethod::GET)([&system_interface]()
                                                                   {
        // 1. Fetch the data from your Manager Object
        // We use a reference (&) to avoid copying thousands of objects unnecessarily
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        auto& all_apps = system_interface.get_storage_obj().get_manager().get_Apps(); 

        crow::json::wvalue response;
        std::vector<crow::json::wvalue> app_list;

        
        // 2. Iterate and convert each C++ struct into a JSON object
        for (const auto& item : all_apps) {
            crow::json::wvalue app_obj;
            if(item.owner_id != system_interface.logged_in_user->dev_id) {
                continue; // Skip apps that don't belong to the logged-in developer
            }
            else if(item.owner_id == system_interface.logged_in_user->dev_id) {
                
                app_obj["app_id"] = item.app_id;     // row[1] in your previous logic
                app_obj["app_name"] = item.name; // row[2]
                app_obj["env"] = item.environment;           // row[3]
            }
            

            app_list.push_back(std::move(app_obj));
        }

        // 3. Return as a JSON array
        return crow::response(crow::json::wvalue(app_list)); });

    CROW_ROUTE(app, "/api/v1/stats/<int>").methods("GET"_method)([&system_interface](int dev_id)
    {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
        // Crow calls the function inside your storage object
        int count = system_interface.Modify_Storage_Obj().Get_Total_Users_For_Dev(dev_id);
            
        crow::json::wvalue res;

        res["total_users"] = count;
        return res; });

    app.port(8080).bindaddr("0.0.0.0").multithreaded().run();

    if (refresh_db.joinable())
    {
        refresh_db.join();
    }
}
