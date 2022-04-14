#pragma once

#include <map>
#include <string>
#include <vector>
#include "ZFile.h"

class FileWorker
{
public:
	std::vector<ZFile*> files;
	std::vector<ZFile*> externalFiles;
	std::vector<int32_t> segments;
	std::map<int32_t, std::vector<ZFile*>> segmentRefFiles;
};