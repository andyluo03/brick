
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

namespace brick {

class log {  // NOLINT
   public:
    log() = delete;
    log(const log&) = delete;
    log& operator=(const log&) = delete;
    log(log&&) = delete;
    log& operator=(log&&) = delete;

    enum class level {
        kDebug = 0,
        kInfo = 1,
        kWarning = 2,
        kError = 3,
        kFatal = 4
    };

    /**
     * @brief Sets the current logging level
     */
    static void set_level(level l);

    template <typename... Types>
    struct debug;

    template <typename... Types>
    struct info;

    template <typename... Types>
    struct warning;

    template <typename... Types>
    struct error;

    template <typename... Types>
    struct fatal;

    // deduction guides
    template <typename... Types>
    debug(Types...) -> debug<Types...>;

    template <typename... Types>
    info(Types...) -> info<Types...>;

    template <typename... Types>
    warning(Types...) -> warning<Types...>;

    template <typename... Types>
    error(Types...) -> error<Types...>;

    template <typename... Types>
    fatal(Types...) -> fatal<Types...>;

   private:
    static level current_level_;
    static std::mutex log_mutex_;

    static const std::string kDebugStr;
    static const std::string kInfoStr;
    static const std::string kWarningStr;
    static const std::string kErrorStr;
    static const std::string kFatalStr;

    static std::string relative_path(const std::string& absolute_path);

    template <typename... Types>
    static void log_args();

    template <typename T, typename... Types>
    static void log_args(T arg1, Types... args);

    template <typename... Types>
    static void generic_log(const std::string& level_str,
                            const std::source_location& location,
                            Types... args);
};

/**
 * HELPERS
 */

/**
 * @brief Base case for variadic logging template
 */
template <typename... Types>
void log::log_args() {}

/**
 * @brief Recursive case for variadic logging template
 * @param arg1 - the first argument to log
 * @param args - the rest of the arguments to log
 */
template <typename T, typename... Types>
void log::log_args(T arg1, Types... args) {
    std::cerr << arg1;
    log_args(args...);
}

/**
 * @brief logs a message using a given level string and source location
 * @param args - the arguments to log
 * @param level_str - the level string to use
 * @param location - the source location of the log
 */
template <typename... Types>
void log::generic_log(const std::string& level_str,
                      const std::source_location& location, Types... args) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    // get time in the format [MM/DD/YYYY HH:MM:SS]
    std::time_t now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cerr << std::put_time(std::localtime(&now), "[%D %T] ");
    std::cerr << level_str << " ";
    // log the arguments
    log_args(args...);
    // print source location
    std::cerr << "    [\033[36m" << relative_path(location.file_name()) << ":"
              << location.line() << "\033[0m] ";
    std::cerr << std::endl;
}

/**
 * PUBLIC API
 */

/**
 * @brief Functor that logs a message at the debug level
 * @param args - the arguments to log
 * @example brick::log::debug("This is a debug message", arg2, arg3, ...);
 */
template <typename... Types>
struct log::debug {  // NOLINT
    explicit debug(Types... args, const std::source_location& location =
                                      std::source_location::current()) {
        if (current_level_ <= level::kDebug) {
            generic_log(kDebugStr, location, args...);
        }
    }
};

/**
 * @brief Functor that logs a message at the info level
 * @param args - the arguments to log, any type that can be streamed to
 * std::cerr
 * @example brick::log::info("This is an info message", arg2, arg3, ...);
 */
template <typename... Types>
struct log::info {  // NOLINT
    explicit info(Types... args, const std::source_location& location =
                                     std::source_location::current()) {
        if (current_level_ <= level::kInfo) {
            generic_log(kInfoStr, location, args...);
        }
    }
};

/**
 * @brief Functor that logs a message at the warning level
 * @param args - the arguments to log, any type that can be streamed to
 * std::cerr
 * @example brick::log::warning("This is a warning message", arg2, arg3, ...);
 */
template <typename... Types>
struct log::warning {  // NOLINT
    explicit warning(Types... args, const std::source_location& location =
                                        std::source_location::current()) {
        if (current_level_ <= level::kWarning) {
            generic_log(kWarningStr, location, args...);
        }
    }
};

/**
 * @brief Functor that logs a message at the error level
 * @param args - the arguments to log, any type that can be streamed to
 * std::cerr
 * @example brick::log::error("This is an error message", arg2, arg3, ...);
 */
template <typename... Types>
struct log::error {  // NOLINT
    explicit error(Types... args, const std::source_location& location =
                                      std::source_location::current()) {
        if (current_level_ <= level::kError) {
            generic_log(kErrorStr, location, args...);
        }
    }
};

/**
 * @brief Functor that logs a message at the fatal level
 * @param args - the arguments to log, any type that can be streamed to
 * std::cerr
 * @example brick::log::fatal("This is a fatal error message", arg2, arg3, ...);
 */
template <typename... Types>
struct log::fatal {  // NOLINT
    explicit fatal(Types... args, const std::source_location& location =
                                      std::source_location::current()) {
        if (current_level_ <= level::kFatal) {
            generic_log(kFatalStr, location, args...);
        }
    }
};

}  // namespace brick

#endif  // UTILS_LOGGER_HPP