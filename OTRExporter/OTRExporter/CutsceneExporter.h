#pragma once
#include "ZResource.h"
#include "ZCutscene.h"
#include "z64cutscene_commands.h"
#include "Exporter.h"

class OTRExporter_Cutscene : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};