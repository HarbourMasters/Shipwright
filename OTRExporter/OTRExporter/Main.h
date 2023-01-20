#pragma once

#include <libultraship/bridge.h>

extern std::shared_ptr<Ship::Archive> otrArchive;
extern std::map<std::string, std::vector<char>> files;

void AddFile(std::string fName, std::vector<char> data);