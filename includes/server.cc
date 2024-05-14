#include "server.hpp"

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include "handler.hpp"
#include "request.hpp"
#include "response.hpp"

namespace brick {

void Server::route(const std::string& path,
                   std::function<Response(Request)> method) {
    router_[path] = std::move(method);
}

void Server::start(int port = 8080) {
    // Make a function for this population...
    port_ = port;

    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd_ <= 0) {
        std::cerr << "Socket creation failed." << std::endl;
        exit(1);
    }

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };

    if (bind(socket_fd_, reinterpret_cast<sockaddr*>(&address),
             sizeof(address)) < 0) {
        std::cerr << "Bind to " << port << " failed." << std::endl;
        exit(1);
    }

    if (listen(socket_fd_, kMaxConnections) < 0) {
        std::cerr << "Listen failed." << std::endl;
        exit(1);
    }

    epoll_fd_ = epoll_create(1);

    if (epoll_fd_ == 0) {
        std::cerr << "What.";
    }

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = socket_fd_;  // why do i use socket_fd twice here

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd_, &event) < 0) {
        std::cerr << "Epoll failed." << std::endl;
        exit(1);
    }

    // populate pool!
    for (int i = 0; i < std::min(1U, std::thread::hardware_concurrency());
         i++) {
        pool_.emplace_back(start_handler, socket_fd_, epoll_fd_, address,
                           router_);
    }
}

Server::~Server() {
    for (std::thread& thread : pool_) {
        thread.join();
    }
    std::cout << "Successfully closed threads." << std::endl;

    close(socket_fd_);
    close(epoll_fd_);
    std::cout << "Successfully shutdown file descriptors." << std::endl;
}
}  // namespace brick
