#pragma once

#include "Utils/BinaryWriter.h"
#include "ZResource.h"
#include "ZTexture.h"

class ExporterExample_Texture : public ZResourceExporter
{
public:
	void Save(ZResource* res, fs::path outPath, BinaryWriter* writer) override;
};