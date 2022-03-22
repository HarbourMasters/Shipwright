#pragma once

#include <string>
#include <vector>

namespace Utils {
    std::vector<std::string> SplitText(const std::string& text, char separator, bool keep_quotes);
}
