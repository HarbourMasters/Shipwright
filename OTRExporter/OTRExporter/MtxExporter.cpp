#include "MtxExporter.h"

void OTRExporter_MtxExporter::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeHeader)
{
	ZMtx* mtx = (ZMtx*)res;

	if (writeHeader)
		WriteHeader(res, writer, Ship::ResourceType::Matrix, Ship::Version::Deckard);

	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			//TODO possibly utilize the array class better
			writer->Write(mtx->mtx[i][j]);
}