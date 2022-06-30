#pragma once

#include "ZBackground.h"
#include "Exporter.h"

class OTRExporter_Background : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
};