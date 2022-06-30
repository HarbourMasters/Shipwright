#pragma once

#include "ZText.h"
#include "Exporter.h"

class OTRExporter_Text : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
};