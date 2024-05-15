#include <cstring>
#include <iostream>
#include <string>

#include "includes/request.hpp"

int main() {
    std::string a =
        "GET / HTTP/1.1\r\n"
        "example: wow\r\n"
        "Content-Type: application/json\r\n"
        "User-Agent: PostmanRuntime/7.31.1\r\n"
        "Accept: */*\r\n"
        "Cache-Control: no-cache\r\n"
        "Postman-Token: 966b1ab0-c71d-46d6-a34a-f687db808f3c\r\n"
        "Host: localhost:3000\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: 10\r\n\r\n"
        "{\r\n"
        "}";

    auto r = brick::Request{a};

    std::cout << r.method() << std::endl;
    std::cout << r.route() << std::endl;
    // std::cout << r.http_version() << std::endl;
    // std::cout << r.header("Host") << std::endl;
    // std::cout << r.header("Content-Type") << std::endl;
    // std::cout << r.header("example") << std::endl;
    std::cout << r.body() << std::endl;
    std::cout << r.body().size() << std::endl;
}