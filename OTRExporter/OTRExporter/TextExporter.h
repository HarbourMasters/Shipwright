#pragma once

#include "ZResource.h"
#include "ZText.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_Text : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};