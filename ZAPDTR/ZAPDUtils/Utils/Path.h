#pragma once

#include <iostream>
#include <string>
#include "Utils/StringHelper.h"

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

class Path
{
public:
	static std::string GetFileName(const fs::path& input)
	{
		// https://en.cppreference.com/w/cpp/filesystem/path/filename
		return input.filename().string();
	};

	static std::string GetFileNameWithoutExtension(const fs::path& input)
	{
		// https://en.cppreference.com/w/cpp/filesystem/path/stem
		return input.stem().string();
	};

	static std::string GetFileNameExtension(const std::string& input)
	{
		return input.substr(input.find_last_of("."), input.length());
	};

	static fs::path GetPath(const std::string& input)
	{
		std::vector<std::string> split = StringHelper::Split(input, "/");
		fs::path output;

		for (std::string str : split)
		{
			if (str.find_last_of(".") == std::string::npos)
				output /= str;
		}

		return output;
	};

	static fs::path GetDirectoryName(const fs::path& path) { return path.parent_path(); };
};
