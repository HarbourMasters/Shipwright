#pragma once

#include "ZBlob.h"
#include "Exporter.h"

class OTRExporter_Blob : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
};