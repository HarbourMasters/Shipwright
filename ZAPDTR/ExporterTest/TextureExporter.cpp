#include "TextureExporter.h"
#include "../ZAPD/ZFile.h"

void ExporterExample_Texture::Save(ZResource* res, [[maybe_unused]] const fs::path& outPath,
                                   BinaryWriter* writer)
{
	ZTexture* tex = (ZTexture*)res;

	auto data = tex->parent->GetRawData();

	for (offset_t i = tex->GetRawDataIndex(); i < tex->GetRawDataIndex() + tex->GetRawDataSize();
	     i++)
		writer->Write(data[i]);
}
