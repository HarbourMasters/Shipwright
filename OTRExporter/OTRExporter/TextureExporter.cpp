#include "TextureExporter.h"
#include "../ZAPD/ZFile.h"

void OTRExporter_Texture::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeHeader)
{
	ZTexture* tex = (ZTexture*)res;
	
	if (writeHeader)
		WriteHeader(tex, writer, Ship::ResourceType::Texture, Ship::Version::Deckard);

	writer->Write((uint32_t)tex->GetTextureType());
	writer->Write((uint32_t)tex->GetWidth());
	writer->Write((uint32_t)tex->GetHeight());

	writer->Write((uint32_t)tex->GetRawDataSize());

	auto data = tex->parent->GetRawData();

	writer->Write((char*)data.data() + tex->GetRawDataIndex(), tex->GetRawDataSize());
}