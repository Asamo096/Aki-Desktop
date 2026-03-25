#pragma once

#include <string>

namespace aki {

namespace utils {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Log {
public:
    static void init();
    static void log(LogLevel level, const std::string &message);
    static void debug(const std::string &message);
    static void info(const std::string &message);
    static void warn(const std::string &message);
    static void error(const std::string &message);
    static void fatal(const std::string &message);
    
    // 性能追踪
    static void trace_start(const std::string &name);
    static void trace_end(const std::string &name);
};

} // namespace utils
} // namespace aki
