#pragma once

#include <string>
#include <vector>

namespace math {
	float clamp(float d, float min, float max);
}

namespace Utils {
	std::vector<std::string> SplitText(const std::string& text, char separator, bool keep_quotes);
}
