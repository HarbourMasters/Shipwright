#pragma once
#include "ZResource.h"
#include "ZArray.h"
//#include "OTRExporter.h"
#include <Utils/BinaryWriter.h>
#include <Resource.h>
#include "VersionInfo.h"

class OTRExporter : public ZResourceExporter
{
protected:
	static void WriteHeader(ZResource* res, BinaryWriter* writer, Ship::ResourceType resType, Ship::Version majorVersion, bool writeFullHeader);
	static Ship::ResourceType GetResourceType(ZResourceType zResType);
};