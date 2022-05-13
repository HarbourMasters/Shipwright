#pragma once

#include "ZResource.h"
#include "ZRoom/ZRoom.h"

class ExporterExample_Room : public ZResourceExporter
{
public:
	void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};