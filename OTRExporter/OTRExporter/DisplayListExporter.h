#pragma once

#include "ZTexture.h"
#include "ZDisplayList.h"
#include "Exporter.h"

class OTRExporter_DisplayList : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
	static std::string GetParentFolderName(ZResource* res);
	static std::string GetPathToRes(ZResource* res, std::string varName);
	static std::string GetPrefix(ZResource* res);

};