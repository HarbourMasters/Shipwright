#include "DisplayList.h"
#include "PR/ultra64/gbi.h"

namespace Ship
{
	void DisplayListV0::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		DisplayList* dl = (DisplayList*)res;

		ResourceFile::ParseFileBinary(reader, res);

		while (reader->GetBaseAddress() % 8 != 0)
			reader->ReadInt8();

		while (true)
		{
			uint32_t w0 = reader->ReadUInt32();
			uint32_t w1 = reader->ReadUInt32();

			if (sizeof(uintptr_t) < 8){
				dl->instructions.push_back(((uint64_t) w0 << 32) | w1);

				uint8_t opcode = w0 >> 24;

				// These are 128-bit commands, so read an extra 64 bits...
				if (opcode == G_SETTIMG_OTR || opcode == G_DL_OTR || opcode == G_VTX_OTR || opcode == G_BRANCH_Z_OTR || opcode == G_MARKER || opcode == G_MTX_OTR) {
					w0 = reader->ReadUInt32();
					w1 = reader->ReadUInt32();

					dl->instructions.push_back(((uint64_t) w0 << 32) | w1);
				}

				if (opcode == G_ENDDL)
					break;
			} else {
				dl->instructions.push_back(w0);
				dl->instructions.push_back(w1);

				uint8_t opcode = (uint8_t)(w0 >> 24);

				if (opcode == G_SETTIMG_OTR || opcode == G_DL_OTR || opcode == G_VTX_OTR || opcode == G_BRANCH_Z_OTR || opcode == G_MARKER || opcode == G_MTX_OTR)
				{
					w0 = reader->ReadUInt32();
					w1 = reader->ReadUInt32();

					dl->instructions.push_back(w0);
					dl->instructions.push_back(w1);
				}

				if (opcode == G_ENDDL)
					break;
			}
		}
	}
}