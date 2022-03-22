#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include "lest.hpp"
#include "mini/ini.h"

using T_LineData = std::vector<std::string>;
using T_INIFileData = std::pair<std::string, T_LineData>;

//
// helper functions
//
bool verifyData(T_INIFileData const& testData)
{
	// compares file contents to expected data
	std::string line;
	std::string const& filename = testData.first;
	T_LineData const& linesExpected = testData.second;
	size_t lineCount = 0;
	size_t lineCountExpected = linesExpected.size();
	std::ifstream fileReadStream(filename);
	if (fileReadStream.is_open())
	{
		while (std::getline(fileReadStream, line))
		{
			if (fileReadStream.bad())
			{
				return false;
			}
			if (lineCount >= lineCountExpected)
			{
				std::cout << "Line count exceeds expected!" << std::endl;
				return false;
			}
			std::string const& lineExpected = linesExpected[lineCount++];
			if (line != lineExpected)
			{
				std::cout << "Line " << lineCount << " does not match expected!" << std::endl;
				std::cout << "Expected: " << lineExpected << std::endl;
				std::cout << "Is: " << line << std::endl;
				return false;
			}
		}
		if (lineCount < lineCountExpected)
		{
			std::cout << "Line count falls behind expected!" << std::endl;
		}
		return lineCount == lineCountExpected;
	}
	return false;
}

//
// test data
//
const T_INIFileData testDataBasic = {
	// filename
	"data01.ini",
	// expected result
	{
		"[section]",
		"key1=value1",
		"key2=value2"
	}
};

const T_INIFileData testDataManySections = {
	// filename
	"data02.ini",
	// expected result
	{
		"[section1]",
		"key1=value1",
		"key2=value2",
		"key3=value3",
		"[section2]",
		"key1=value1",
		"key2=value2",
		"[section3]",
		"key1=value1"
	}
};

const T_INIFileData testDataEmptySection = {
	// filename
	"data03.ini",
	// expected result
	{
		"[empty]"
	}
};

const T_INIFileData testDataManyEmptySections = {
	// filename
	"data04.ini",
	// expected result
	{
		"[empty1]",
		"[empty2]",
		"[empty3]"
	}
};

const T_INIFileData testDataEmpty = {
	// filename
	"data05.ini",
	// expected result
	{}
};

const T_INIFileData testDataPrettyPrint = {
	// filename
	"data06.ini",
	// expected result
	{
		"[section1]",
		"key1 = value1",
		"key2 = value2",
		"",
		"[section2]",
		"key1 = value1"
	}
};

const T_INIFileData testDataEmptyNames = {
	// filename
	"data07.ini",
	// expected result
	{
		"[]",
		"key=value",
		"[section]",
		"=value",
		"[section2]",
		"="
	}
};

const T_INIFileData testDataMalformed1 = {
	// filename
	"data08.ini",
	// test data
	{
		"[[name1]",
		"key=value",
		"[name2]]",
		"key=value",
		"[[name3]]",
		"key=value"
	}
};

const T_INIFileData testDataMalformed2 = {
	// filename
	"data09.ini",
	// test data
	{
		"[name]",
		"\\===",         // key: "="   value: "="
		"a\\= \\===b",  //  key: "a= ="  value: "=b"
		"c\\= \\===d"  //  key: "c= ="  value: "=d"
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("Test: Basic generate")
	{
		// create a very basic INI file and verify resulting file has correct data
		std::string const& filename = testDataBasic.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini["section"]["key1"] = "value1";
		ini["section"]["key2"] = "value2";
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataBasic) == true);
	},
	CASE("Test: Generate many sections")
	{
		std::string const& filename = testDataManySections.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini["section1"].set({
			{"key1", "value1"},
			{"key2", "value2"},
			{"key3", "value3"}
		});
		ini["section2"].set({
			{"key1", "value1"},
			{"key2", "value2"}
		});
		ini["section3"]["key1"] = "value1";
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataManySections));
	},
	CASE("Test: Generate empty section")
	{
		std::string const& filename = testDataEmptySection.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini["empty"];
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataEmptySection));
	},
	CASE("Test: Generate many empty sections")
	{
		std::string const& filename = testDataManyEmptySections.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini["empty1"];
		ini["empty2"];
		ini["empty3"];
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataManyEmptySections));
	},
	CASE("Test: Generate empty file")
	{
		std::string const& filename = testDataEmpty.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataEmpty));
	},
	CASE("Test: Generate with pretty-print")
	{
		std::string const& filename = testDataPrettyPrint.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini["section1"].set({
			{"key1", "value1"},
			{"key2", "value2"},
		});
		ini["section2"]["key1"] = "value1";
		EXPECT(file.generate(ini, true) == true);
		EXPECT(verifyData(testDataPrettyPrint));
	},
	CASE("Test: Generate empty section and key names")
	{
		std::string const& filename = testDataEmptyNames.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini[""]["key"] = "value";
		ini["section"][""] = "value";
		ini["section2"][""] = "";
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataEmptyNames));
	},
	CASE("Test: Generate malformed section names")
	{
		std::string const& filename = testDataMalformed1.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini["[name1"]["key"] = "value";
		ini["name2]"]["key"] = "value";
		ini["[name3]"]["key"] = "value";
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataMalformed1));
	},
	CASE("Test: Generate malformed key names")
	{
		std::string const& filename = testDataMalformed2.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini["name"].set({
			{"=", "="},
			{"a= =", "=b"},
			{"     c= =  ", "    =d  "}
		});
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataMalformed2));
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

