#pragma once
#include <string>
#include "Span.h"

Span<char> ReadAllBytes(const std::string& filePath);