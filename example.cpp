#include "includes/server.hpp"

int main () {
    auto a = brick::Server();
    
    //apparently chaining is actually kinda hard...
    //a.route("Hello", [](const std::string& a){ return a + "hello_world"; });

    a.start(3000);
}