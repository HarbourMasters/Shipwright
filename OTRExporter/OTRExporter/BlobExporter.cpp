#include "BlobExporter.h"
#include "../ZAPD/ZFile.h"

void OTRExporter_Blob::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeHeader)
{
	ZBlob* blob = (ZBlob*)res;
	
	if (writeHeader)
		WriteHeader(blob, writer, Ship::ResourceType::Blob, Ship::Version::Deckard);

	writer->Write((uint32_t)blob->GetRawDataSize());

	auto data = blob->parent->GetRawData();

	for (size_t i = blob->GetRawDataIndex(); i < blob->GetRawDataIndex() + blob->GetRawDataSize(); i++)
		writer->Write(data[i]);
}