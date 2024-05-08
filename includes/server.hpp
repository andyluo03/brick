#include <thread>
#include <vector>
#include <string>

namespace brick {

class Server {
 public:
  Server() = default;
  ~Server();
  
  Server& route(const std::string& path, std::function<std::string(std::string)> method);
  void start(int socket);

 private:
  std::unordered_map<std::string, std::function<std::string(std::string)>> router_;
  std::vector<std::thread> pool_;
};
}