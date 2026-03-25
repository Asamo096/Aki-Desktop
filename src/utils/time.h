#pragma once

#include <cstdint>

namespace aki {

namespace utils {

class Time {
public:
    static uint64_t get_monotonic_us();
    static uint64_t get_monotonic_ms();
};

} // namespace utils
} // namespace aki
