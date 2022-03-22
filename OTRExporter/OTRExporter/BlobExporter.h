#pragma once

#include "ZResource.h"
#include "ZBlob.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_Blob : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};