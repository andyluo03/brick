#include <thread>
#include <string>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <cstring>

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "handler.hpp"
#include "server.hpp"
#include "request.hpp"
#include "response.hpp"

namespace brick {
  void Server::route(const std::string& path, std::function<Response(Request)> method){
    router_[path] = std::move(method);
  }

  
  void Server::start(int port = 8080){
    port_ = port;

    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_fd_ <= 0){
      std::cerr << "Socket creation failed." << std::endl;
      exit(1);
    }

    struct sockaddr_in address = {
      .sin_family = AF_INET,
      .sin_port = htons(port),
    };

    if(bind(socket_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0){
      std::cerr << "Bind to " << port << " failed." << std::endl;
      exit(1);
    }

    if(listen(socket_fd_, kMaxConnections) < 0){
      std::cerr << "Listen failed." << std::endl;
      exit(1);
    }

    epoll_fd_ = epoll_create(1);

    if(epoll_fd_ == 0){
      std::cerr << "What.";
    }

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = socket_fd_; // why do i use socket_fd twice here

    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd_, &event) < 0) {
      std::cerr << "Epoll failed." << std::endl;
      exit(1);
    }
    
    struct epoll_event events[kMaxConnections]; //dont want to keep reallocating!


    start_handler(socket_fd_, epoll_fd_, address);
    /**
    while(true){
      int event_size = epoll_wait(epoll_fd_, events, kMaxConnections, -1);

      for(int i = 0; i < event_size; i++){
        socklen_t stupid = sizeof(address);
        int client_fd = //accept( socket_fd_, reinterpret_cast<sockaddr*>(&address), &stupid);
          events[0].data.fd; //<< " " << std::endl;

        accept( socket_fd_, reinterpret_cast<sockaddr*>(&address), &stupid);

        //std::cout << 

        std::cout << client_fd << " " << socket_fd_ << std::endl;
        char buffer[1000];
        int sz = read(events[0].data.fd, buffer, 999);

        std::string asd;
        for(int i = 0; i < sz; i++){
          asd+=buffer[i];
        }

        Request a(asd);

        std::cout << a.body() << "!" << std::endl;
        Response resp;
        resp.set_body(a.body());

        std::string fff = resp.build();
        const char* ex = fff.c_str();
        write(client_fd, ex, sizeof(char) * fff.size());
        std::cout << buffer << std::endl;
        close(client_fd);
      }
    }
    **/
    
  }

  Server::~Server(){
    std::cout << "Closing threads." << std::endl;
    for(std::thread& thread : pool_){
      thread.join();
    }

    std::cout << "Closing sockets." << std::endl;
    close(socket_fd_);
    close(epoll_fd_);

    std::cout << "Successful shutdown." << std::endl;
  }
}
