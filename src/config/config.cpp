#include "aki/config/config.h"
#include "aki/utils/log.h"

#include <fstream>
#include <sstream>

namespace aki {

namespace config {

Config::Config() {
}

Config::~Config() {
}

bool Config::load(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        utils::Log::error("Failed to open config file: " + path);
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // 简单的键值对解析
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            values[key] = value;
        }
    }

    utils::Log::info("Config loaded from: " + path);
    return true;
}

std::string Config::get(const std::string &key, const std::string &default_value) const {
    auto it = values.find(key);
    if (it != values.end()) {
        return it->second;
    }
    return default_value;
}

int Config::get_int(const std::string &key, int default_value) const {
    auto it = values.find(key);
    if (it != values.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return default_value;
        }
    }
    return default_value;
}

float Config::get_float(const std::string &key, float default_value) const {
    auto it = values.find(key);
    if (it != values.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return default_value;
        }
    }
    return default_value;
}

bool Config::get_bool(const std::string &key, bool default_value) const {
    auto it = values.find(key);
    if (it != values.end()) {
        std::string value = it->second;
        if (value == "true" || value == "1") {
            return true;
        } else if (value == "false" || value == "0") {
            return false;
        }
    }
    return default_value;
}

} // namespace config
} // namespace aki
