#pragma once

#include "ZResource.h"
#include "ZTexture.h"
#include "ZAnimation.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_Animation : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};