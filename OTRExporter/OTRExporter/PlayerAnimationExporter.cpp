#include "PlayerAnimationExporter.h"
#include <Resource.h>

void OTRExporter_PlayerAnimationExporter::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeHeader)
{
	ZPlayerAnimationData* anim = (ZPlayerAnimationData*)res;

	if (writeHeader)
		WriteHeader(res, writer, Ship::ResourceType::PlayerAnimation, Ship::Version::Deckard);

	writer->Write((uint32_t)anim->limbRotData.size());

	for (size_t i = 0; i < anim->limbRotData.size(); i++)
		writer->Write(anim->limbRotData[i]);
}
