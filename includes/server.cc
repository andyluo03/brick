#include <thread>
#include <string>
#include <sys/epoll.h>

#include "server.hpp"

namespace brick {
  Server& Server::route(const std::string& path, std::function<std::string(std::string)> method){
    router_[path] = std::move(method);
    return *this;
  }

  void Server::start(int socket){

  }

  Server::~Server(){
    for(std::thread& thread : pool_){
        thread.join();
    }
  }
}