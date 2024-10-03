#include "includes/handler.hpp"
#include "includes/server.hpp"
#include "utils/logger.hpp"

brick::Response mirror_body(const brick::Request& a) {
    brick::Response t(200);
    t.set_body(a.body());
    return t;
}

brick::Response hello_world(const brick::Request& a) {
    brick::Response t(200);
    t.set_body("Hello, World!");
    return t;
}

// example of a useful route
brick::Response show_request(const brick::Request& a) {
    brick::Response t(200);
    std::string body = "Method: " + a.method() + "\n";
    body += "Route: " + a.route() + "\n";
    body += "HTTP Version: " + a.http_version() + "\n";
    body += "----------------\n";
    body += "Headers:\n";
    for (const auto& [key, value] : a.headers()) {
        body += key + ": " + value + "\n";  // NOLINT
    }
    body += "----------------\n";
    body += "Body: " + a.body() + "\n";
    t.set_body(body);
    return t;
}

int main() {
    brick::log::set_level(brick::log::level::kInfo);
    brick::log::debug("Set log level to debug");
    auto a = brick::Server(8);

    // apparently chaining is actually kinda hard...
    a.route("/mirror", mirror_body);
    a.route("/", hello_world);
    a.route("/show", show_request);
    a.start(3000);
}
