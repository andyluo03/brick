#ifndef B_RESPONSE
#define B_RESPONSE

#include <map>
#include <string>
#include <string_view>
#include <unordered_map>

namespace brick {

class Response {
   public:
    /**
     * @brief Constructor for Response
     * @param `status_code` the status code of the response
     */
    explicit Response(unsigned int status_code);

    /**
     * @brief Build raw response string
     * @return response string
     */
    std::string raw();

    /**
     * @brief Set the body of the response
     * @param `body` the body of the response (as a string)
     */
    void set_body(const std::string& body);

    // /**
    //  * @brief Set the body of the response
    //  * @param `body` the body of the response (as a string_view)
    //  */
    // void set_body(const std::string_view& body);

    /**
     * @brief Set a header in the response
     * @param `key` the key of the header
     * @param `value` the value of the header
     */
    void set_header(const std::string& key, const std::string& value);

    // ** accessors **

    /**
     * @brief Get the status code of the response
     * @return status code
     */
    unsigned int status_code() const { return status_code_; }

    /**
     * @brief Get the body of the response
     * @return body
     */
    std::string body() const { return body_; }

    /**
     * @brief Get a header from the response
     * @param `key` the key of the header
     * @return header value
     */
    std::string header(const std::string& key) const {
        return headers_.at(key);
    }

     /**
     * Map of status codes to status message strings
     */
    static const std::map<unsigned int, std::string> kStatusMessages;

   private:
    std::string body_;
    unsigned int status_code_;
    std::map<std::string, std::string> headers_; 
};

}  // namespace brick
#endif