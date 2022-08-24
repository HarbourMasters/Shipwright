#pragma once

#include <string>
#include <vector>

namespace Ship {

	namespace Math {
		float clamp(float d, float min, float max);
	}

	std::vector<std::string> SplitText(const std::string& text, char separator, bool keep_quotes);
	std::string toLowerCase(std::string in);
}