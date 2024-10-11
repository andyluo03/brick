#pragma once

#include <map>
#include <string>
#include <string_view>

namespace brick {
class Request {
   public:
    /**
     * @brief Constructor for empty Request
     */
    Request() = default;

    /**
     * @brief Constructor for Request
     * @param `request` the raw request string
     */
    explicit Request(const std::string& request);

    // accessors

    /**
     * @brief Get a header from the request
     * @param `header` the header to get
     * @return header value
     */
    std::string header(const std::string& header) const {
        return headers_.at(header);
    }

    /**
     * @brief Get all headers from the request
     * @return headers
     */
    std::map<std::string, std::string> headers() const { return headers_; }

    /**
     * @brief Get the method of the request
     * @return method
     */
    std::string method() const { return method_; }

    /**
     * @brief Get the HTTP version of the request
     * @return HTTP version
     */
    std::string http_version() const { return http_version_; }

    /**
     * @brief Get the body of the request
     * @return body
     */
    std::string body() const { return body_; }

    /**
     * @brief Get the raw request string
     * @return raw request string
     */
    std::string raw() const { return request_; }

    /**
     * @brief Get the route of the request
     * @return route
     */
    std::string route() const { return route_; }

    ~Request() = default;

   private:
    std::string request_;
    unsigned int size_;

    std::string method_;
    std::string route_;
    std::string http_version_;
    std::string body_;
    std::map<std::string, std::string> headers_;
};
}  // namespace brick
