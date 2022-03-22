#pragma once

#include "ZResource.h"
#include "ZCollision.h"
#include "Exporter.h"

class OTRExporter_Collision : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};