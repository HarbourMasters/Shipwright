#include "resource/importer/TextFactory.h"
#include "resource/type/Text.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> TextFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<Text>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<TextFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load Text with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::TextFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<Text> text = std::static_pointer_cast<Text>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	uint32_t msgCount = reader->ReadUInt32();
	text->messages.reserve(msgCount);

	for (uint32_t i = 0; i < msgCount; i++)
	{
		MessageEntry entry;
		entry.id = reader->ReadUInt16();
		entry.textboxType = reader->ReadUByte();
		entry.textboxYPos = reader->ReadUByte();
		entry.msg = reader->ReadString();

		text->messages.push_back(entry);
	}
}
} // namespace Ship
