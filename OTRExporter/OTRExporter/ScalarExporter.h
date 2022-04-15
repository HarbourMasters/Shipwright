#pragma once

/* 
* Changelog:
* V1 (Roy) - Initial implementation
*/


#include "ZResource.h"
#include "ZVtx.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

// Note: Ultimately this exporter goes *mostly* unused. Because of the lack of single vertex elements, the Array Exporter is used instead, 
// which calls our serialization function. So it remains here mostly as a placeholder and may be deleted altogether in the future.

class OTRExporter_Scalar : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
	static void WriteScalarType(ZScalar* scalar, BinaryWriter* writer);
};