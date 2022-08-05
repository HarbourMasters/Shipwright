#include "Text.h"

void Ship::TextV0::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader)
{
	Text* txt = (Text*)res;

	ResourceFile::ParseFileBinary(reader, txt, readFullHeader);

	uint32_t msgCount = reader->ReadUInt32();
	txt->messages.reserve(msgCount);

	for (uint32_t i = 0; i < msgCount; i++)
	{
		MessageEntry entry;
		entry.id = reader->ReadUInt16();
		entry.textboxType = reader->ReadUByte();
		entry.textboxYPos = reader->ReadUByte();
		entry.msg = reader->ReadString();

		txt->messages.push_back(entry);
	}
}
