#include "Cutscene.h"

void Ship::CutsceneV0::ParseFileBinary(BinaryReader* reader, Resource* res)
{
	Cutscene* cs = (Cutscene*)res;

	ResourceFile::ParseFileBinary(reader, res);

	uint32_t numEntries = reader->ReadUInt32();
	cs->commands.reserve(numEntries);

	for (uint32_t i = 0; i < numEntries; i++)
	{
		uint32_t data = reader->ReadUInt32();
		uint16_t opcode = data >> 16;

		cs->commands.push_back(data);
	}
}
