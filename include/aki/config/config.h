#pragma once

#include <string>
#include <unordered_map>

namespace aki {

namespace config {

class Config {
private:
    std::unordered_map<std::string, std::string> values;

public:
    Config();
    ~Config();

    bool load(const std::string &path);
    std::string get(const std::string &key, const std::string &default_value = "") const;
    int get_int(const std::string &key, int default_value = 0) const;
    float get_float(const std::string &key, float default_value = 0.0f) const;
    bool get_bool(const std::string &key, bool default_value = false) const;
};

} // namespace config
} // namespace aki
