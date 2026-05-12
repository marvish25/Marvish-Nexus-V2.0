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
    CROW_ROUTE(app, "/static/<string>")
    ([](std::string filename)
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

    CROW_ROUTE(app, "/favicon.ico")([]
                                    { return ""; });

    CROW_ROUTE(app, "/Developer.html")([]
                                       {
        std::lock_guard<std::mutex> lock(mtx);
        auto page = crow::mustache::load("Developer.html");
        return crow::response(page.render()); });

    // ... rest of My routes ...
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

    CROW_ROUTE(app, "/api/v1/system/audit-stream")([]()
                                                   {
        std::ifstream file("nexus_audit.log");
        std::vector<std::string> log_lines;
        std::string line;
        while (std::getline(file, line)) {
            log_lines.push_back(line);
            if (log_lines.size() > 20) log_lines.erase(log_lines.begin());
        }
        crow::json::wvalue x;
        x["logs"] = std::move(log_lines);
        return x; });

    CROW_ROUTE(app, "/api/v1/login").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)

        ([&](const crow::request &req)
         {
    crow::response res;
    
    std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access to logs and system resources
    log.log_nexus_activity(req, "LOGIN_ATTEMPT");
    // Manually attach headers here to be 100% sure
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");

    if (req.method == crow::HTTPMethod::OPTIONS) {
        res.code = 200;
        res.body = "OK";
        return res;
    }

    auto body = crow::json::load(req.body);
    if (!body) {
        res.code = 400;
        res.body = "JSON_ERROR";
        return res;
    }

    
    Status result = system_interface.LogIn(body["app_id"].s(), body["id"].s(), body["pw"].s());

    if (result == SUCCESS) {
        res.code = 200;
        res.body = "AUTHORIZED";
    } else {
        res.code = 401;
        res.body = "DENIED";
    }
    
    return res; });
    
    CROW_ROUTE(app, "/api/v1/register").methods("POST"_method)([&system_interface](const crow::request &req)
                                                               {
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

            std::cout << "[DEBUG] Received registration data - Username: " << username << ", Email: " << email << "\n";
            std::cout <<"[DEBUG] Password and Confirm Password received. Lengths: " << password.length() << ", " << confirmPw.length() << "\n";
            std::cout <<"[DEBUG] Checking password match: " << (password == confirmPw ? "MATCH" : "NO MATCH") << "\n";
            std::cout << "[DEBUG] Checking password strength for: " << password << "\n";
            

            // Interfacing with the Marvish Nexus Kernel
            Status result = system_interface.Sign_In(username, email, password, confirmPw); 

            if (result == Status::SUCCESS) {
                crow::json::wvalue res;
                res["status"] = "STABILIZED";
                res["msg"] = "Operator Profile Committed to Nexus.";
                return crow::response(200, res);
            } else if(result == Status::ERR_USER_FOUND) {
                return crow::response(302, "User already exists.");
            } else {
                return crow::response(400, "Weak Password : Must be at least 8 characters, include uppercase, lowercase, and a number.");
            }
        } catch (const std::exception& e) {
            return crow::response(500, "KERNEL_PANIC: Data Extraction Failed.");
        } });

    CROW_ROUTE(app, "/api/v1/users")([&]()
                                     {
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
        std::string cmd = "sudo arp-scan --localnet";
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return crow::response(500, "Scan Failed");
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) result += buffer;
        pclose(pipe);
        return crow::response(200, result); });

    CROW_ROUTE(app, "/api/v1/deploy").methods("POST"_method)([&system_interface](const crow::request &req)
                                                             {
        auto x = crow::json::load(req.body);
    
        // Safety check to prevent Kernel Panic
        if (!x.has("app_name") || !x.has("app_id")) return crow::response(400);

        // 1. Extract
        std::string name = x["app_name"].s();
        std::string id   = x["app_id"].s();
        std::string env  = x["env"].s();
        std::string key  = "SK_LIVE_" + id; // You can generate this randomly

        // 2. Get the logged-in dev's ID (this should come from your login session)
        int current_dev = 1; 

        // 3. Create Object and Store
        NexusApp new_deployment(current_dev, name, id, key, env);
    
        if (system_interface.Modify_Storage_Obj().Modify_Manager_Obj().Register_New_App(new_deployment)) {
            return crow::response(200, "APP_DEPLOYED_SUCCESSFULLY");
        }
    
        return crow::response(500, "DEPLOYMENT_REJECTED_BY_DATABASE"); 
    });
    CROW_ROUTE(app, "/api/v1/stats/<int>").methods("GET"_method)
        ([&system_interface](int dev_id) {
        // Crow calls the function inside your storage object
        int count = system_interface.Modify_Storage_Obj().Modify_Manager_Obj().Get_Total_Users_For_Dev(dev_id);
            
        crow::json::wvalue res;
        res["total_users"] = count;
        return res;
    });

      app.port(8080).bindaddr("0.0.0.0").multithreaded().run();

        if (refresh_db.joinable())
        {   
            refresh_db.join();
        }   
}
