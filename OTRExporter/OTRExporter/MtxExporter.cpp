#include "MtxExporter.h"

void OTRExporter_MtxExporter::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZMtx* mtx = (ZMtx*)res;

	WriteHeader(res, outPath, writer, LUS::ResourceType::Matrix);

	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			//TODO possibly utilize the array class better
			writer->Write(mtx->mtx[i][j]);
}