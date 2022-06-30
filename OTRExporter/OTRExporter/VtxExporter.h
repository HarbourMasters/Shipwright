#pragma once

/* 
* Changelog:
* V0 (Deckard) - Initial implementation
* V1 (Roy) - Removed count field. Now only holds one vertex, like it should have originally. The lists always had one element anyways so it was redundant...
*/


#include "ZVtx.h"
#include "Exporter.h"

// Note: Ultimately this exporter goes *mostly* unused. Because of the lack of single vertex elements, the Array Exporter is used instead, 
// which calls our serialization function. So it remains here mostly as a placeholder and may be deleted altogether in the future.

class OTRExporter_Vtx : public OTRExporter
{
public:
	static void WriteVtx(ZVtx* vtx, BinaryWriter* writer);
	void SaveArr(ZResource* res, const fs::path& outPath, const std::vector<ZResource*>&, BinaryWriter* writer, bool writeFullHeader);
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader) override;
};