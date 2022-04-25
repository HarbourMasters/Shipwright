#pragma once

#include "ZTexture.h"
#include "Exporter.h"

class OTRExporter_Texture : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
};