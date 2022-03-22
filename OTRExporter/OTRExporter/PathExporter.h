#pragma once

#include "ZResource.h"
#include "ZPath.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_Path : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};