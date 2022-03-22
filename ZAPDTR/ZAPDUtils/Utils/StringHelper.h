#pragma once

#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <numeric>
#include <string>
#include <vector>

class StringHelper
{
public:
	static std::vector<std::string> Split(std::string s, const std::string& delimiter);
	static std::string Strip(std::string s, const std::string& delimiter);
	static std::string Replace(std::string str, const std::string& from, const std::string& to);
	static void ReplaceOriginal(std::string& str, const std::string& from, const std::string& to);
	static bool StartsWith(const std::string& s, const std::string& input);
	static bool Contains(const std::string& s, const std::string& input);
	static bool EndsWith(const std::string& s, const std::string& input);
	static std::string Sprintf(const char* format, ...);
	static std::string Implode(std::vector<std::string>& elements, const char* const separator);
	static int64_t StrToL(const std::string& str, int32_t base = 10);
	static std::string BoolStr(bool b);
	static bool HasOnlyDigits(const std::string& str);
	static bool IEquals(const std::string& a, const std::string& b);
};