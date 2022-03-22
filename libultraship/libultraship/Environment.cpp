#include "Environment.h"

#include <map>
#include <string>

std::map<std::string, std::string> environmentVars;

namespace SohUtils {
    void saveEnvironmentVar(const std::string& key, const std::string& value) {
        environmentVars[key] = value;
    }
    std::string getEnvironmentVar(const std::string& key) {
        return environmentVars[key];
    }
}