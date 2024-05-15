
#ifndef UTILS_LOGGER_HPP
#define UTILS_LOGGER_HPP

#include <iostream>
#include <mutex>
#include <source_location>

/**
 * This is a simple logging utility that provides a way to log messages at
 * different levels.
 *
 * The levels are as follows (from highest to lowest priority)
 *       - fatal
 *       - error
 *       - warning
 *       - info
 *       - debug
 *
 * The current logging level can be set using the `set_level` function. By
 * default, the logging level is set to info. The logging level can be set to
 * any of the levels above, and all messages at that level and above will be
 * logged (e.g. if the level is set to warning, all warning, error, and fatal
 * messages will be logged).
 *
 * The logging functions support any number of arguments, and will print them to
 * stderr in the order they are passed.
 *
 * Usage:
 *       brick::log::{level}(arg1, arg2, ...);
 *
 * Examples:
 *       brick::log::info("This is an info message");
 *       brick::log::debug("Iterating over ", 10, " elements");
 *       brick::log::error("An error occurred with error message: ",
 * strerror(errno));
 */

namespace brick::log {

enum class level {
    kDebug = 0,
    kInfo = 1,
    kWarning = 2,
    kError = 3,
    kFatal = 4
};

const std::string kDebugStr = "\033[34m[DEBUG]\033[0m";
const std::string kInfoStr = "\033[32m[INFO]\033[0m";
const std::string kWarningStr = "\033[33m[WARNING]\033[0m";
const std::string kErrorStr = "\033[31m[ERROR]\033[0m";
const std::string kFatalStr = "\033[31m[FATAL]\033[0m";

extern level current_level;
extern std::mutex log_mutex;

std::string relative_path(const std::string& absolute_path);

/**
 * @brief Sets the current logging level
 */
void set_level(level l);

/**
 * @brief Base case for variadic logging template
 */
template <typename... Types>
void log_args() {}

/**
 * @brief Recursive case for variadic logging template
 * @param arg1 - the first argument to log
 * @param args - the rest of the arguments to log
 */
template <typename T, typename... Types>
void log_args(T arg1, Types... args) {
    std::cerr << arg1;
    log_args(args...);
}

/**
 * @brief Logs a message at the debug level
 * @param args - the arguments to log
 */
template <typename... Types>
struct debug {  // NOLINT
    explicit debug(Types... args, const std::source_location& location =
                                      std::source_location::current()) {
        if (current_level <= level::kDebug) {
            std::lock_guard<std::mutex> lock(log_mutex);
            std::cerr << "[" << relative_path(location.file_name()) << ":"
                      << location.line() << "] " << kDebugStr << " ";
            log_args(args...);
            std::cerr << std::endl;
        }
    }
};
// deduction guide
template <typename... Types>
debug(Types...) -> debug<Types...>;

/**
 * @brief Logs a message at the info level
 * @param args - the arguments to log
 */
template <typename... Types>
struct info {  // NOLINT
    explicit info(Types... args, const std::source_location& location =
                                     std::source_location::current()) {
        if (current_level <= level::kInfo) {
            std::lock_guard<std::mutex> lock(log_mutex);
            std::cerr << "[" << relative_path(location.file_name()) << ":"
                      << location.line() << "] " << kInfoStr << " ";
            log_args(args...);
            std::cerr << std::endl;
        }
    }
};
// deduction guide
template <typename... Types>
info(Types...) -> info<Types...>;

/**
 * @brief Logs a message at the warning level
 * @param args - the arguments to log
 */
template <typename... Types>
struct warning {  // NOLINT
    explicit warning(Types... args, const std::source_location& location =
                                        std::source_location::current()) {
        if (current_level <= level::kWarning) {
            std::lock_guard<std::mutex> lock(log_mutex);
            std::cerr << "[" << relative_path(location.file_name()) << ":"
                      << location.line() << "] " << kWarningStr << " ";
            log_args(args...);
            std::cerr << std::endl;
        }
    }
};
// deduction guide
template <typename... Types>
warning(Types...) -> warning<Types...>;

/**
 * @brief Logs a message at the error level
 * @param args - the arguments to log
 */
template <typename... Types>
struct error {  // NOLINT
    explicit error(Types... args, const std::source_location& location =
                                      std::source_location::current()) {
        if (current_level <= level::kError) {
            std::lock_guard<std::mutex> lock(log_mutex);
            std::cerr << "[" << relative_path(location.file_name()) << ":"
                      << location.line() << "] " << kErrorStr << " ";
            log_args(args...);
            std::cerr << std::endl;
        }
    }
};
// deduction guide
template <typename... Types>
error(Types...) -> error<Types...>;

/**
 * @brief Logs a message at the fatal level
 * @param args - the arguments to log
 */
template <typename... Types>
struct fatal {  // NOLINT
    explicit fatal(Types... args, const std::source_location& location =
                                      std::source_location::current()) {
        if (current_level <= level::kFatal) {
            std::lock_guard<std::mutex> lock(log_mutex);
            std::cerr << "[" << relative_path(location.file_name()) << ":"
                      << location.line() << "] " << kFatalStr << " ";
            log_args(args...);
            std::cerr << std::endl;
        }
    }
};
// deduction guide
template <typename... Types>
fatal(Types...) -> fatal<Types...>;

}  // namespace brick::log

#endif  // UTILS_LOGGER_HPP