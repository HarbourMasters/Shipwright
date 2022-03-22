/* use testhuge -t to time tests */

#include <iostream>
#include "lest.hpp"
#include "mini/ini.h"

const std::string filename = "data_huge.ini";

const size_t N_sections = 20;
const size_t N_items_per_section = 500;

const bool generate_pretty = false;

const lest::test mINI_tests[] = {
	CASE("TEST: Generate a huge file")
	{
		std::cout << "Writing file..." << std::endl;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		// generate data
		for (unsigned int i = 1; i <= N_sections; ++i)
		{
			std::string section = "section" + std::to_string(i);
			for (unsigned int j = 1; j <= N_items_per_section; ++j)
			{
				std::string j_str = std::to_string(j);
				std::string key = "key" + j_str;
				std::string value = "value" + j_str;
				ini[section][key] = value;
			}
		}
		// generate file
		EXPECT(file.generate(ini) == true);
	},
	CASE("TEST: Read a huge file")
	{
		// this testcase relies on the previous test passing
		std::cout << "Reading file..." << std::endl;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		// read from file
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == N_sections);
		for (auto& it : ini)
		{
			EXPECT(it.second.size() == N_items_per_section);
		}
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
