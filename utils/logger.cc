#include "logger.hpp"

namespace brick {

log::level log::current_level = log::level::kInfo;
std::mutex log::log_mutex;  


void log::set_level(log::level new_level) {
    std::lock_guard<std::mutex> lock(log_mutex);
    current_level = new_level;
}

// probably a better way to do this
std::string log::relative_path(const std::string& absolute_path) {
    size_t pos = absolute_path.find("brick/");
    if (pos == std::string::npos) {
        return absolute_path;
    }
    return absolute_path.substr(pos + 6);
}

}  // namespace brick::log
