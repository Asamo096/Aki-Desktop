#include "aki/utils/time.h"

#include <chrono>

namespace aki {

namespace utils {

uint64_t Time::get_monotonic_us() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

uint64_t Time::get_monotonic_ms() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

} // namespace utils

} // namespace aki
