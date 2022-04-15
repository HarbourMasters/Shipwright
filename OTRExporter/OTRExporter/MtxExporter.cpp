#include "MtxExporter.h"

void OTRExporter_MtxExporter::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader)
{
	ZMtx* mtx = (ZMtx*)res;

	WriteHeader(res, writer, Ship::ResourceType::Matrix, Ship::Version::Deckard, writeFullHeader);

	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			//TODO possibly utilize the array class better
			writer->Write(mtx->mtx[i][j]);
}