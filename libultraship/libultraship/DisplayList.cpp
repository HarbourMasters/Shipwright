#include "DisplayList.h"
#include "PR/ultra64/gbi.h"

namespace Ship
{
	void DisplayListV0::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		DisplayList* dl = (DisplayList*)res;

		ResourceFile::ParseFileBinary(reader, res);

		while (reader->GetBaseAddress() % 8 != 0)
			reader->ReadByte();

		while (true)
		{
			uint64_t data = reader->ReadUInt64();

			if (sizeof(uintptr_t) < 8){
				dl->instructions.push_back(data);

				uint8_t opcode = data >> 24;

				// These are 128-bit commands, so read an extra 64 bits...
				if (opcode == G_SETTIMG_OTR || opcode == G_DL_OTR || opcode == G_VTX_OTR || opcode == G_BRANCH_Z_OTR || opcode == G_MARKER || opcode == G_MTX_OTR)
					dl->instructions.push_back(reader->ReadUInt64());

				if (opcode == G_ENDDL)
					break;
			} else {
				uint32_t w0 = (uint32_t)data;
				uint32_t w1 = (uint32_t)(data >> 32);

				dl->instructions.push_back(w0);
				dl->instructions.push_back(w1);

				uint8_t opcode = (uint8_t)(w0 >> 24);

				if (opcode == G_SETTIMG_OTR || opcode == G_DL_OTR || opcode == G_VTX_OTR || opcode == G_BRANCH_Z_OTR || opcode == G_MARKER || opcode == G_MTX_OTR)
				{
					data = reader->ReadUInt64();

					w0 = (uint32_t)data;
					w1 = (uint32_t)(data >> 32);

					dl->instructions.push_back(w0);
					dl->instructions.push_back(w1);
				}

				if (opcode == G_ENDDL)
					break;
			}
		}
	}
}