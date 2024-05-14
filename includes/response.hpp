#ifndef B_RESPONSE
#define B_RESPONSE

#include <string>
#include <string_view>

namespace brick {

class Response {
  public:
    Response() = default;

    std::string build();
    void set_body(const std::string& body);
    void set_body(const std::string_view& body);

  private:
    std::string body_;
    std::string status_;
};

}
#endif