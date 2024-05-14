#include "response.hpp"

namespace brick {

std::string Response::build(){
    std::string result;
    result += "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n";
    result += body_;
    return result;
}

void Response::set_body(const std::string& body){
    body_ = body;
}

void Response::set_body(const std::string_view& body){
    body_ = "";
    for(char i : body){
        body_ += i;
    }
}

}