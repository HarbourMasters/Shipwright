#pragma once

#include <string>
#include <vector>
#include "StringHelper.h"
#include <iostream>

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#undef GetCurrentDirectory
#undef CreateDirectory

class Directory
{
public:
	#ifndef PATH_HACK
	static std::string GetCurrentDirectory() { return fs::current_path().string(); }
	#endif

	static bool Exists(const fs::path& path) { return fs::exists(path); }

	// Stupid hack because of Windows.h
	static void MakeDirectory(const std::string& path)
	{
		CreateDirectory(path);
	}

	static void CreateDirectory(const std::string& path)
	{
		try
		{
			fs::create_directories(path);
		}
		catch (...)
		{
		}
	}

	static std::vector<std::string> ListFiles(const std::string& dir)
	{
		std::vector<std::string> lst;

		if (Exists(dir))
		{
			for (auto& p : fs::recursive_directory_iterator(dir))
			{
				if (!p.is_directory())
					lst.push_back(p.path().generic_string());
			}
		}

		return lst;
	}
};
