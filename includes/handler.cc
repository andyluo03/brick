#include "handler.hpp"
#include "response.hpp"

#include <iostream> 

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void brick::start_handler(int port_fd, int epoll_fd, sockaddr_in& address){
    socklen_t addr_size = sizeof(address);
    while(true){
        struct epoll_event client[10];
        std::cout << epoll_wait(epoll_fd, client, 10, -1);
        
        struct epoll_event a;

        int relevant_fd = client[0].data.fd;
        //accept(epoll_fd, reinterpret_cast<sockaddr*>(&address), &addr_size);
        
        if(client[0].data.fd == port_fd){
            std::cerr << "new connection!" << std::endl;
            a.events = EPOLLIN;

            int new_fd = accept(port_fd, reinterpret_cast<sockaddr*>(&address), &addr_size);
            
            a.data.fd = new_fd;
            a.events = EPOLLIN | EPOLLET;
            if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &a) < 0){
                std::cerr << "bad " << new_fd;
                exit(1);
            }
        }else{
            std::cerr << "answering connection!" << std::endl;
            char buf[1000];
            int size = read(relevant_fd, buf, 999);
            buf[size] = '\0';

            Response t;
            t.set_body(std::string(buf));
            std::cerr << t.build() << std::endl;

            write(relevant_fd, t.build().c_str(), t.build().size());

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