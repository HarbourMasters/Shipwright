#pragma once

#include <string>

namespace SohUtils {
    void saveEnvironmentVar(const std::string& key, const std::string& value);
    bool hasEnvironmentVar(const std::string& key);
    std::string getEnvironmentVar(const std::string& key);
}