#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include "lest.hpp"
#define MINI_CASE_SENSITIVE
#include "mini/ini.h"

using T_LineData = std::vector<std::string>;
using T_INIFileData = std::pair<std::string, T_LineData>;

//
// helper functions
//
bool writeTestFile(T_INIFileData const& testData)
{
	std::string const& filename = testData.first;
	T_LineData const& lines = testData.second;
	std::ofstream fileWriteStream(filename);
	if (fileWriteStream.is_open())
	{
		if (lines.size())
		{
			auto it = lines.begin();
			for (;;)
			{
				fileWriteStream << *it;
				if (++it == lines.end())
				{
					break;
				}
				fileWriteStream << std::endl;
			}
		}
		return true;
	}
	return false;
}

void outputData(mINI::INIStructure const& ini)
{
	for (auto const& it : ini)
	{
		auto const& section = it.first;
		auto const& collection = it.second;
		std::cout << "[" << section << "]" << std::endl;
		for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			std::cout << key << "=" << value << std::endl;
		}
		std::cout << std::endl;
	}
}

//
// test data
//
const T_INIFileData testDataBasic = {
	// filename
	"data01.ini",
	// test data
	{
		"[Fruit]",
		"Bananas=1",
		"APPLES=2",
	}
};

const lest::test mINI_tests[] = {
	CASE("Test: Read case sensitive")
	{
		// read a basic INI file and check if values are read correctly
		auto const& filename = testDataBasic.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		std::cout << filename << std::endl;
		outputData(ini);
		EXPECT(ini["Fruit"]["Bananas"] == "1");
		EXPECT(ini["Fruit"]["APPLES"] == "2");
		EXPECT(ini["fruit"]["bananas"] == "");
		EXPECT(ini["fruit"]["apples"] == "");
	},
};

int main(int argc, char** argv)
{
	// write test files
	writeTestFile(testDataBasic);

	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}