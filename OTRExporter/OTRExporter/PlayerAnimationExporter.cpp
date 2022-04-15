#include "PlayerAnimationExporter.h"
#include <Resource.h>

void OTRExporter_PlayerAnimationExporter::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader)
{
	ZPlayerAnimationData* anim = (ZPlayerAnimationData*)res;

	WriteHeader(res, writer, Ship::ResourceType::PlayerAnimation, Ship::Version::Deckard, writeFullHeader);

	writer->Write((uint32_t)anim->limbRotData.size());

	for (size_t i = 0; i < anim->limbRotData.size(); i++)
		writer->Write(anim->limbRotData[i]);
}
