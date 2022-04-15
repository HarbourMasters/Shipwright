#include "BlobExporter.h"
#include "../ZAPD/ZFile.h"

void OTRExporter_Blob::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader)
{
	ZBlob* blob = (ZBlob*)res;
	
	WriteHeader(blob, writer, Ship::ResourceType::Blob, Ship::Version::Deckard, writeFullHeader);

	writer->Write((uint32_t)blob->GetRawDataSize());

	auto data = blob->parent->GetRawData();

	for (size_t i = blob->GetRawDataIndex(); i < blob->GetRawDataIndex() + blob->GetRawDataSize(); i++)
		writer->Write(data[i]);
}