#pragma once
#include "ZResource.h"
#include "ZArray.h"
#include "stdint.h"
//#include "OTRExporter.h"
#include <Utils/BinaryWriter.h>
#include <libultraship/bridge.h>
#include "VersionInfo.h"

class OTRExporter : public ZResourceExporter
{
protected:
	static void WriteHeader(ZResource* res, const fs::path& outPath, BinaryWriter* writer, LUS::ResourceType resType, int32_t resVersion = 0);
};