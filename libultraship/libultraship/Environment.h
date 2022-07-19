#pragma once

#include <string>

namespace SohUtils {
    void saveEnvironmentVar(const std::string& key, const std::string& value);
    std::string getEnvironmentVar(const std::string& key);
}