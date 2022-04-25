#pragma once

#include "ZMtx.h"
#include "Exporter.h"

class OTRExporter_MtxExporter : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
};
