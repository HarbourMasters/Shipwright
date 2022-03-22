#include "PlayerAnimation.h"
#include "PR/ultra64/gbi.h"

namespace Ship
{
	void PlayerAnimationV0::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		PlayerAnimation* anim = (PlayerAnimation*)res;

		ResourceFile::ParseFileBinary(reader, res);

		uint32_t numEntries = reader->ReadUInt32();
		anim->limbRotData.reserve(numEntries);

		for (uint32_t i = 0; i < numEntries; i++)
			anim->limbRotData.push_back(reader->ReadInt16());

	}
}