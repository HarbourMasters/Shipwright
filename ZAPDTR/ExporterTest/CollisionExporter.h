#pragma once

#include "ZCollision.h"
#include "ZResource.h"

class ExporterExample_Collision : public ZResourceExporter
{
public:
	void Save(ZResource* res, fs::path outPath, BinaryWriter* writer) override;
};