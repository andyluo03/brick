#include "server.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

#include "handler.hpp"
#include "logger.hpp"
#include "request.hpp"
#include "response.hpp"

#define MAX_REQUEST_SIZE (10 * 1024)  // 10KB, including null terminator

namespace brick {

// volatile sig_atomic_t serving_ = 1; // NOLINT

void Server::route(const std::string& path,
                   std::function<Response(Request)> method) {
    router_[path] = std::move(method);
}

void Server::start(int port = 8080) {
    init(port);
    // threads should not receive SIGINT or SIGPIPE
    block_signals();

    log::info("Running on port ", port, " with ", pool_.capacity(), " threads");

    // populate pool!
    for (int i = 0; i < pool_.capacity(); i++) {
        pool_.emplace_back(&Server::process_events, this);
    }

    // handle sigints using sigwait
    sigset_t set;
    int sig;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, nullptr);

    while (serving_) {
        sigwait(&set, &sig);
        if (sig == SIGINT) {
            log::info("Received SIGINT, stopping server...");
            serving_ = 0;
        }
    }

    cleanup();

    // // instead of sigwait, unblock SIGINT and create a sigaction
    // sigset_t set;
    // sigemptyset(&set);
    // sigaddset(&set, SIGINT);
    // pthread_sigmask(SIG_UNBLOCK, &set, nullptr);
    // struct sigaction sa;
    // sa.sa_handler = [] (int sig){
    //     log::info("Received ", strsignal(sig), ", stopping server...");
    //     serving_ = 0;
    // };
    // sigaction(SIGINT, &sa, nullptr);
}

void Server::process_events() {
    struct epoll_event events[kMaxConnections];
    int nfds;

    while (serving_) {
        // timeout is so that we won't say blocked on `epoll_wait` forever after
        // SIGINT log::debug("Thread ", std::this_thread::get_id(), " waiting
        // for events..."); // NOLINT
        nfds = epoll_wait(epoll_fd_, events, kMaxConnections, 1000);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            log::fatal("epoll_wait() failed: ", strerror(errno));
            exit(1);
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == socket_fd_) {
                accept_connection();
            } else {
                handle_request(events[i].data.fd);
            }
        }
    }
}

void Server::accept_connection() const {
    int client_fd = accept(socket_fd_, nullptr, nullptr);
    if (client_fd < 0) {
        log::error("Failed to accept connection: ", strerror(errno));
        return;
    }

    // TODO(akhil): set client_fd to non-blocking

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_fd;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
        log::error("Failed to add client to epoll: ", strerror(errno));
        remove_client(client_fd);
    }

    log::debug("Thread ", std::this_thread::get_id(),
               " accepted connection on fd ", client_fd);
}

void Server::handle_request(int client_fd) {
    char buf[MAX_REQUEST_SIZE];
    int size = recv(client_fd, buf, MAX_REQUEST_SIZE - 1, 0);
    if (size < 0) {
        log::error("Failed to read from client: ", strerror(errno));
        remove_client(client_fd);
        return;
    }
    buf[size] = '\0';

    // build request
    Request request(buf);

    // build response
    auto endpoint = std::string(request.route());
    // Response response = router_.at(endpoint)(incoming_request);
    Response response(404);
    if (router_.find(endpoint) != router_.end()) {
        response = router_.at(endpoint)(request);
        log::info("\"", request.method(), " ", request.route(), "\" ",
                  response.status_code(), " ",
                  Response::kStatusMessages.at(response.status_code()));
    } else {
        log::error("\"", request.method(), " ", request.route(), "\" ",
                   response.status_code(), " ",
                   Response::kStatusMessages.at(response.status_code()));
    }

    std::string ans = response.build();

    // send response
    if (send(client_fd, ans.c_str(), ans.size(), 0) < 0) {
        log::error("Failed to send response: ", strerror(errno));
        remove_client(client_fd);
        return;
    }

    remove_client(client_fd);
}

void Server::remove_client(int client_fd) const {
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr);
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
}

void Server::init(int port) {
    init_listener(port);
    init_epoll();
}

void Server::init_listener(int port) {
    // create socket
    // TODO(akhil): use nonblocking sockets instead
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        log::fatal("Failed to create socket: ", strerror(errno));
        exit(1);
    }

    // set options
    int opt = 1;
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
        0) {
        log::fatal("Failed to set socket options: ", strerror(errno));
        exit(1);
    }
    opt = 1;
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) <
        0) {
        log::fatal("Failed to set socket options: ", strerror(errno));
        exit(1);
    }

    // get address info
    struct addrinfo hints, *res;  // NOLINT
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status =
        getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &res);
    if (status != 0) {
        log::fatal("Failed to get address info: ", gai_strerror(status));
        exit(1);
    }

    // bind socket
    if (bind(socket_fd_, res->ai_addr, res->ai_addrlen) < 0) {
        log::fatal("Failed to bind socket: ", strerror(errno));
        exit(1);
    }

    // listen
    if (listen(socket_fd_, kMaxConnections) < 0) {
        log::fatal("Failed to listen: ", strerror(errno));
        exit(1);
    }
}

void Server::init_epoll() {
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ < 0) {
        log::fatal("Failed to create epoll: ", strerror(errno));
        exit(1);
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = socket_fd_;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd_, &event) < 0) {
        log::fatal("Failed to add socket to epoll: ", strerror(errno));
        exit(1);
    }
}

void Server::block_signals() {
    // threads should not receive SIGINT or SIGPIPE
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &set, nullptr);
}

void Server::cleanup() {
    log::info("Cleaning up server...");
    for (std::thread& thread : pool_) {
        thread.join();
    }
    // log::info("Cleaning up server...");
    log::info("Successfully closed threads");
    shutdown(socket_fd_, SHUT_RDWR);
    close(socket_fd_);
    close(epoll_fd_);
    log::info("Successfully shutdown server");
}

// Constructor and Destructor

Server::Server(unsigned int num_threads) {
    log::debug("Creating server with ", num_threads, " threads");
    pool_.reserve(num_threads);
}

// Server::~Server() {
//     cleanup();
// }

}  // namespace brick
