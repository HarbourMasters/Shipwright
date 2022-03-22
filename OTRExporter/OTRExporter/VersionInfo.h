#pragma once

#include <map>
#include <vector>
#include "Resource.h"

#define MAJOR_VERSION Ship::Version::Deckard

extern std::map<Ship::ResourceType, uint32_t> resourceVersions;