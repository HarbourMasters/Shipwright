#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Directory.h"
#include "Utils/StringHelper.h"

class File
{
public:
	static bool Exists(const fs::path& filePath)
	{
		std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
		return file.good();
	}

	static std::vector<uint8_t> ReadAllBytes(const fs::path& filePath)
	{
		std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
		int32_t fileSize = (int32_t)file.tellg();
		file.seekg(0);
		char* data = new char[fileSize];
		file.read(data, fileSize);
		std::vector<uint8_t> result = std::vector<uint8_t>(data, data + fileSize);
		delete[] data;

		return result;
	};

	static std::string ReadAllText(const fs::path& filePath)
	{
		std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
		if (!file.is_open())
			return "";
		int32_t fileSize = (int32_t)file.tellg();
		file.seekg(0);
		char* data = new char[fileSize + 1];
		memset(data, 0, fileSize + 1);
		file.read(data, fileSize);
		std::string str = std::string((const char*)data);
		delete[] data;

		return str;
	};

	static std::vector<std::string> ReadAllLines(const fs::path& filePath)
	{
		std::string text = ReadAllText(filePath);
		std::vector<std::string> lines = StringHelper::Split(text, "\n");

		return lines;
	};

	static void WriteAllBytes(const fs::path& filePath, const std::vector<uint8_t>& data)
	{
		std::ofstream file(filePath, std::ios::binary);
		file.write((char*)data.data(), data.size());
	};

	static void WriteAllBytes(const std::string& filePath, const std::vector<char>& data)
	{
		std::ofstream file(filePath, std::ios::binary);
		file.write((char*)data.data(), data.size());
	};

	static void WriteAllBytes(const std::string& filePath, const char* data, int dataSize)
	{
		std::ofstream file(filePath, std::ios::binary);
		file.write((char*)data, dataSize);
	};

	static void WriteAllText(const fs::path& filePath, const std::string& text)
	{
		std::ofstream file(filePath, std::ios::out);
		file.write(text.c_str(), text.size());
	}
};
