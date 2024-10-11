
#ifndef UTILS_LOGGER_HPP
#define UTILS_LOGGER_HPP

#include <iomanip>
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
enum class level { kDebug, kInfo, kWarning, kError, kFatal };

/**
 * @brief Current logging level (default is info) - set using `set_level` (don't
 * modify directly)
 */
extern level current_level;

/**
 * @brief Mutex to ensure thread-safe logging
 */
extern std::mutex log_mutex;

/**
 * @brief Convert absolute path to relative path (portion of path after
 * 'brick/')
 */
std::string relative_path(const std::string& absolute_path);

namespace {  // NOLINT
/*
 * Private functions and variables
 */

const std::string kDebugStr = "\033[34m[DEBUG]\033[0m";
const std::string kInfoStr = "\033[32m[INFO]\033[0m";
const std::string kWarningStr = "\033[33m[WARNING]\033[0m";
const std::string kErrorStr = "\033[31m[ERROR]\033[0m";
const std::string kFatalStr = "\033[31m[FATAL]\033[0m";

/**
 * @brief Base case for variadic logging template
 */
template <typename... Types>
void log_args() {}

/**
 * @brief Recursive case for variadic logging template
 */
template <typename T, typename... Types>
void log_args(T&& arg, Types&&... args) {
    std::cerr << std::forward<T>(arg);
    log_args(std::forward<Types>(args)...);
}

/**
 * @brief logs a message using a given level string and source location
 * @param args - the arguments to log
 * @param level_str - the level string to use
 * @param location - the source location of the log
 */
template <typename... Types>
void log_message(const std::string& level_str,
                 const std::source_location& location, Types&&... args) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::time_t now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cerr << std::put_time(std::localtime(&now), "[%D %T] ");
    std::cerr << std::left << std::setw(20) << level_str << " ";

    // print source location
    std::cerr << std::right << std::setw(35)
              << "\033[36m" + relative_path(location.file_name()) + ":" +
                     std::to_string(location.line()) + "\033[0m";
    
    std::cerr << " -- ";
    // log the arguments
    log_args(std::forward<Types>(args)...);

    /*// print source location*/
    /*std::cerr << "\t [\033[36m" << relative_path(location.file_name()) <<
     * ":"*/
    /*          << location.line() << "\033[0m] ";*/
    std::cerr << std::endl;
}

}  // namespace

/**
 * PUBLIC API
 */

/**
 * @brief Set the current logging level
 */
void set_level(level new_level);

/**
 * @brief Functor that logs a message at the debug level
 * @param args - the arguments to log
 * @example brick::log::debug("This is a debug message", arg2, arg3, ...);
 */
template <typename... Types>
struct debug {  // NOLINT
    explicit debug(Types... args, std::source_location location =
                                      std::source_location::current()) {
        if (current_level <= level::kDebug) {
            log_message(kDebugStr, location, std::forward<Types>(args)...);
        }
    }
};
// deduction guide
template <typename... Types>
debug(Types...) -> debug<Types...>;

/**
 * @brief Functor that logs a message at the info level
 * @param args - the arguments to log
 * @example brick::log::info("This is an info message", arg2, arg3, ...);
 */
template <typename... Types>
struct info {  // NOLINT
    explicit info(Types... args, std::source_location location =
                                     std::source_location::current()) {
        if (current_level <= level::kInfo) {
            log_message(kInfoStr, location, std::forward<Types>(args)...);
        }
    }
};

// deduction guide
template <typename... Types>
info(Types...) -> info<Types...>;

/**
 * @brief Functor that logs a message at the warning level
 * @param args - the arguments to log
 * @example brick::log::warning("This is a warning message", arg2, arg3, ...);
 */
template <typename... Types>
struct warning {  // NOLINT
    explicit warning(Types... args, std::source_location location =
                                        std::source_location::current()) {
        if (current_level <= level::kWarning) {
            log_message(kWarningStr, location, std::forward<Types>(args)...);
        }
    }
};

// deduction guide
template <typename... Types>
warning(Types...) -> warning<Types...>;

/**
 * @brief Functor that logs a message at the error level
 * @param args - the arguments to log
 * @example brick::log::error("This is an error message", arg2, arg3, ...);
 */
template <typename... Types>
struct error {  // NOLINT
    explicit error(Types... args, std::source_location location =
                                      std::source_location::current()) {
        if (current_level <= level::kError) {
            log_message(kErrorStr, location, std::forward<Types>(args)...);
        }
    }
};

// deduction guide
template <typename... Types>
error(Types...) -> error<Types...>;

/**
 * @brief Functor that logs a message at the fatal level
 * @param args - the arguments to log
 * @example brick::log::fatal("This is a fatal message", arg2, arg3, ...);
 */
template <typename... Types>
struct fatal {  // NOLINT
    explicit fatal(Types... args, std::source_location location =
                                      std::source_location::current()) {
        if (current_level <= level::kFatal) {
            log_message(kFatalStr, location, std::forward<Types>(args)...);
        }
    }
};

// deduction guide
template <typename... Types>
fatal(Types...) -> fatal<Types...>;

}  // namespace brick::log

#endif  // UTILS_LOGGER_HPP
