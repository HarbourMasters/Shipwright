#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include "lest.hpp"
#include "mini/ini.h"

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
// test cases
//
const lest::test mINI_tests[] = {
	CASE("Test: Write and read back utf-8 values")
	{
		mINI::INIFile file("data01.ini");
		mINI::INIStructure ini;
		ini["section"]["key"] = "€";
		ini["section"]["€"] = "value";
		ini["€"]["key"] = "value";
		ini["section"]["key2"] = "𐍈";
		ini["section"]["𐍈"] = "value";
		ini["𐍈"]["key"] = "value";
		ini["section"]["key3"] = "你好";
		ini["section"]["你好"] = "value";
		ini["你好"]["key"] = "value";
		EXPECT(file.write(ini) == true);
		ini.clear();
		EXPECT(file.read(ini) == true);
		outputData(ini);
		EXPECT(ini["section"]["key"] == "€");
		EXPECT(ini["section"]["key2"] == "𐍈");
		EXPECT(ini["section"]["€"] == "value");
		EXPECT(ini["€"]["key"] == "value");
		EXPECT(ini["section"]["𐍈"] == "value");
		EXPECT(ini["𐍈"]["key"] == "value");
		EXPECT(ini["section"]["key3"] == "你好");
		EXPECT(ini["section"]["你好"] == "value");
		EXPECT(ini["你好"]["key"] == "value");
	}
};

int main(int argc, char** argv)
{
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}

