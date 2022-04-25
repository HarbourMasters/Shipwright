#pragma once

#include "ZTexture.h"
#include "ZDisplayList.h"
#include "ZSkeleton.h"
#include "ZLimb.h"
#include "Exporter.h"

class OTRExporter_SkeletonLimb : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
};