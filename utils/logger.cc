#include "logger.hpp"

namespace brick::log {

level current_level = level::kInfo;
std::mutex log_mutex;

void set_level(level l) {
    std::lock_guard<std::mutex> lock(log_mutex);
    current_level = l;
}

// probably a better way to do this
std::string relative_path(const std::string& absolute_path) {
    // project directory is brick/
    return absolute_path.substr(absolute_path.find("brick/"));
}

}  // namespace brick::log
