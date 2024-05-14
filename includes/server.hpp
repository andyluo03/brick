#ifndef B_SERVER
#define B_SERVER

#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "request.hpp"
#include "response.hpp"

namespace brick {

class Server {
   public:
    Server() = default;
    ~Server();

    void route(const std::string& path,
               std::function<Response(Request)> method);
    void start(int port);
    void build_pool(int threads);

   private:
    // thread-safe on read...
    static constexpr unsigned int kMaxConnections = 10000;

    std::unordered_map<std::string, std::function<Response(Request)>> router_;
    std::vector<std::thread> pool_;
    int socket_fd_;
    int port_;
    int epoll_fd_;
};
}  // namespace brick

#endif
