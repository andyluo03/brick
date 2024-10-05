#ifndef B_SERVER
#define B_SERVER

#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <csignal>

#include "request.hpp"
#include "response.hpp"

namespace brick {

class Server {
   public:
    explicit Server(unsigned int num_threads = std::thread::hardware_concurrency());
    // ~Server();
    // delete copy, move, and copy assignment
    Server(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(const Server&) = delete;

    void route(const std::string& path,
               std::function<Response(Request)> method);
    void start(int port);

   private:
    void init(int port);
    void init_listener(int port);
    void init_epoll();
    static void block_signals();

    void process_events();
    void accept_connection() const;
    void handle_request(int client_fd);
    void remove_client(int client_fd) const;
    void cleanup();

    // thread-safe on read...
    static constexpr unsigned int kMaxConnections = 10000;

    std::unordered_map<std::string, std::function<Response(Request)>> router_;
    std::vector<std::thread> pool_;
    int socket_fd_;
    int port_;
    int epoll_fd_;
    volatile sig_atomic_t serving_ = 1;
};
}  // namespace brick

#endif
