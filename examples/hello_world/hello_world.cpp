#include "brick/server/server.hpp"
#include "brick/request/request.hpp"
#include "brick/response/response.hpp"

#include <iostream>


brick::Response mirror_body(const brick::Request& a) {
    brick::Response t{200};
    t.set_body(a.body());
    return t;
}

brick::Response hello_world(const brick::Request&) {
    brick::Response t{200};
    t.set_body("Hello, World!");
    return t;
}

int main() {
    auto a = brick::Server(8);

    a.route("/mirror", "POST", mirror_body);
    a.route("/", "GET", hello_world);
    a.start(3000);
}
