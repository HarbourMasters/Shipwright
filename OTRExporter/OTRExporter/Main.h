#pragma once

#include <Archive.h>

void AddFile(std::string fName, std::vector<char> data);

extern std::shared_ptr<Ship::Archive> otrArchive;
extern std::map<std::string, std::vector<char>> files;

void AddFile(std::string fName, std::vector<char> data);