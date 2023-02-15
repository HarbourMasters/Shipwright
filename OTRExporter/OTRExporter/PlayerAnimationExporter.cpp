#include "PlayerAnimationExporter.h"
#include <libultraship/bridge.h>

void OTRExporter_PlayerAnimationExporter::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZPlayerAnimationData* anim = (ZPlayerAnimationData*)res;

	WriteHeader(res, outPath, writer, Ship::ResourceType::SOH_PlayerAnimation);

	auto start = std::chrono::steady_clock::now();
	
	writer->Write((uint32_t)anim->limbRotData.size());

	for (size_t i = 0; i < anim->limbRotData.size(); i++)
		writer->Write(anim->limbRotData[i]);

	auto end = std::chrono::steady_clock::now();
	size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	//printf("Exported Player Anim %s in %zums\n", anim->GetName().c_str(), diff);
}
