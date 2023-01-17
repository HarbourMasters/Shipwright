#include "BackgroundExporter.h"
#include "../ZAPD/ZFile.h"

void OTRExporter_Background::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZBackground* bg = (ZBackground*)res;

	WriteHeader(bg, outPath, writer, Ship::ResourceType::SOH_Background);

	writer->Write((uint32_t)bg->GetRawDataSize());
	
	auto data = bg->parent->GetRawData();
	writer->Write((char*)data.data() + bg->GetRawDataIndex(), bg->GetRawDataSize());
}