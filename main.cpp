#include "routes.hpp"

int main(){
    
    routes r;
    crow::App<CorsMiddleware> app;
    
    r.setup_routes(app);
    
}