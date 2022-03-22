#pragma once

#include "ZResource.h"
#include "ZBackground.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_Background : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};