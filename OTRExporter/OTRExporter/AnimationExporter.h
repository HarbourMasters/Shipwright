#pragma once

#include "ZAnimation.h"
#include "Exporter.h"

class OTRExporter_Animation : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
};