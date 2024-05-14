#ifndef B_REQUEST
#define B_REQUEST

#include <map>
#include <string>
#include <string_view>

namespace brick {
class Request {
   public:
    Request() = delete;

    // DO NOT EXPOSE THIS TO THE USER.
    explicit Request(std::string_view request);

    std::string_view header(const std::string& header);
    std::string_view method();
    std::string_view http_version();
    std::string_view body();
    std::string_view raw();
    std::string_view route();

    ~Request() = default;

   private:
    std::string request_;
    unsigned int size_;

    std::string_view method_;
    std::string_view route_;
    std::string_view http_version_;
    std::string_view body_;
    std::map<std::string, std::string_view> headers_;
};
}  // namespace brick

#endif