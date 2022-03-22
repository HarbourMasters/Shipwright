#pragma once
#include "ZResource.h"
#include "ZArray.h"
//#include "OTRExporter.h"
#include <Utils/BinaryWriter.h>
#include <Resource.h>

class OTRExporter : public ZResourceExporter
{
protected:
	static void WriteHeader(ZResource* res, const fs::path& outPath, BinaryWriter* writer, Ship::ResourceType resType);
};