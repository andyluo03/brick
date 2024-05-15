#include "handler.hpp"

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "response.hpp"

void brick::start_handler(
    int port_fd, int epoll_fd, sockaddr_in address,
    const std::unordered_map<std::string, std::function<Response(Request)>>&
        router) {
    socklen_t addr_size = sizeof(address);
    while (true) {
        struct epoll_event client[10];
        std::cout << epoll_wait(epoll_fd, client, 10, -1);
        struct epoll_event a;
        int relevant_fd = client[0].data.fd;

        if (client[0].data.fd == port_fd) {
            std::cerr << "new connection!" << std::endl;
            a.events = EPOLLIN;

            int new_fd = accept(port_fd, reinterpret_cast<sockaddr*>(&address),
                                &addr_size);

            a.data.fd = new_fd;
            a.events = EPOLLIN | EPOLLET;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &a) < 0) {
                std::cerr << "bad " << new_fd;
                exit(1);
            }
        } else {
            std::cerr << "answering connection!" << std::endl;
            char buf[10000];
            int size = read(relevant_fd, buf, 9999);
            buf[size] = '\0';

            Request incoming_request(buf);
            Response response = router.at(
                std::string(incoming_request.route()))(incoming_request);

            std::string ans = response.build();
            write(relevant_fd, ans.c_str(), ans.size());
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, relevant_fd, &a);
            close(relevant_fd);
        }
    }
}

/**
static inline int handle_client(int client_fd, int epoll_fd){

}

static inline int handle_new_connection(int port_fd, int epoll_fd){

}
*/