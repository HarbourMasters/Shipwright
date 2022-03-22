#include <iostream>
#include "lest.hpp"
#include "mini/ini.h"

bool compareData(mINI::INIStructure a, mINI::INIStructure b)
{
	for (auto const& it : a)
	{
		auto const& section = it.first;
		auto const& collection = it.second;
		if (collection.size() != b[section].size()) {
			return false;
		}
		for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			if (value != b[section][key]) {
				return false;
			}
		}
		std::cout << std::endl;
	}
	return a.size() == b.size();
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

const lest::test mINI_tests[] = {
	CASE("TEST: Copy semantics")
	{
		mINI::INIStructure iniA;
		
		iniA["a"].set({
			{ "x", "1" },
			{ "y", "2" },
			{ "z", "3" }
		});
		
		iniA["b"].set({
			{ "q", "100" },
			{ "w", "100" },
			{ "e", "100" }
		});
		
		mINI::INIStructure iniB(iniA);
		EXPECT(compareData(iniA, iniB));
		
		mINI::INIStructure iniC = iniA;
		EXPECT(compareData(iniA, iniC));
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