#include "request.hpp"

#include <cstring>
#include <iostream>
#include <limits>
#include <numeric>
#include <string_view>

namespace brick {

enum HttpParseState { kMethod, kUri, kHttpType, kHeaderName, kHeaderValue };

/*
GET /path HTTP/1.1\r\n
Host: localhost:8080\r\n
Content-Type: application/json\r\n
Content-Length: 36\r\n
\r\n
*/

// There might be some bugs w/ incorrect requests...
Request::Request(const std::string& request) : request_(request) {
    size_ = request.size();

    HttpParseState state = kMethod;

    std::string key_buffer;
    int low = 0;
    for (int i = 0; i < size_; i++) {
        char unit = request[i];

        if (unit == '\0') {
            std::cerr << "?!?!?!?!?!?";
            break;
        }

        switch (state) {
            case kMethod:
                if (unit == ' ') {
                    state = kUri;
                    method_ = request.substr(low, i - low);

                    low = i + 1;
                    continue;
                }

                break;

            case kUri:
                if (unit == ' ') {
                    route_ = request.substr(low, i - low);

                    low = i + 1;
                    state = kHttpType;
                }

                break;

            case kHttpType:
                if (i >= low + 3 && unit == '\n' && request[i - 1] == '\r') {
                    http_version_ = request.substr(low, i - 1 - low);
                    low = i + 1;
                    // i++;
                    state = kHeaderName;
                }

                break;

            case kHeaderName:
                if (i >= low + 1 && unit == '\n' && request[i - 1] == '\r') {
                    low = i + 1;
                    i = std::numeric_limits<int>::max();
                }

                if (unit == ':') {
                    // create a state for this (later)
                    i++;
                    while (i < size_ && request[i] == ' ') {
                        i++;
                    }

                    state = kHeaderValue;
                    low = i;
                } else {
                    key_buffer += unit;
                }

                break;

            case kHeaderValue:
                if (i >= low + 1 && unit == '\n' && request[i - 1] == '\r') {
                    headers_[key_buffer] = request.substr(low, i - 1 - low);
                    key_buffer = "";
                    state = kHeaderName;
                    low = i + 1;
                }

                break;
        }
    }

    if (low < size_) body_ = request.substr(low, size_ - low);
}

}  // namespace brick
