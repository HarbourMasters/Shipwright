#pragma once

#include "ZResource.h"
#include "ZTexture.h"
#include "ZDisplayList.h"
#include "ZSkeleton.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_Skeleton : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};