#include "aki/utils/log.h"
#include "aki/utils/time.h"
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <unordered_map>

namespace aki {

namespace utils {

static LogLevel current_level = LogLevel::INFO;
static bool initialized = false;
static std::unordered_map<std::string, std::chrono::steady_clock::time_point> trace_starts;

void Log::init() {
    // 从环境变量读取日志级别
    const char* log_level_env = std::getenv("AKI_LOG");
    if (log_level_env) {
        std::string level_str = log_level_env;
        if (level_str == "debug") {
            current_level = LogLevel::DEBUG;
        } else if (level_str == "info") {
            current_level = LogLevel::INFO;
        } else if (level_str == "warn") {
            current_level = LogLevel::WARN;
        } else if (level_str == "error") {
            current_level = LogLevel::ERROR;
        } else if (level_str == "fatal") {
            current_level = LogLevel::FATAL;
        }
    }
    
    initialized = true;
    log(LogLevel::INFO, "Logger initialized");
}

std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") 
       << "." << std::setw(3) << std::setfill('0') << ms.count();
    return ss.str();
}

std::string level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

void Log::log(LogLevel level, const std::string &message) {
    if (level < current_level) {
        return;
    }
    
    std::string timestamp = get_timestamp();
    std::string level_str = level_to_string(level);
    
    std::cout << "[" << timestamp << "] [" << level_str << "] " << message << std::endl;
}

void Log::debug(const std::string &message) {
    log(LogLevel::DEBUG, message);
}

void Log::info(const std::string &message) {
    log(LogLevel::INFO, message);
}

void Log::warn(const std::string &message) {
    log(LogLevel::WARN, message);
}

void Log::error(const std::string &message) {
    log(LogLevel::ERROR, message);
}

void Log::fatal(const std::string &message) {
    log(LogLevel::FATAL, message);
}

void Log::trace_start(const std::string &name) {
    trace_starts[name] = std::chrono::steady_clock::now();
    debug("Trace started: " + name);
}

void Log::trace_end(const std::string &name) {
    auto it = trace_starts.find(name);
    if (it != trace_starts.end()) {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - it->second);
        debug("Trace ended: " + name + " - Duration: " + std::to_string(duration.count()) + "ms");
        trace_starts.erase(it);
    } else {
        warn("Trace end called for non-existent trace: " + name);
    }
}

} // namespace utils

} // namespace aki
