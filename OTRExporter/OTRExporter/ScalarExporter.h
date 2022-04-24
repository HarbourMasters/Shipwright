#pragma once

/* 
* Changelog:
* V1 (Roy) - Initial implementation
*/


#include "ZResource.h"
#include "ZVtx.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_Scalar : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
	static void WriteScalarType(ZScalar* scalar, BinaryWriter* writer);
};