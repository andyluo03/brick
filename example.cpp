#include "includes/handler.hpp"
#include "includes/server.hpp"

brick::Response mirror_body(brick::Request a) {
    brick::Response t(200);
    t.set_body(a.body());
    return t;
}

brick::Response hello_world(brick::Request a) {
    a.body();
    brick::Response t(200);
    t.set_body(std::string("Hello World"));
    return t;
}

int main() {
    auto a = brick::Server();

    // apparently chaining is actually kinda hard...
    a.route("/mirror", mirror_body);
    a.route("/", hello_world);
    a.start(3000);
}