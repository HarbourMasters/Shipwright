#include "soh/resource/importer/TextFactory.h"
#include "soh/resource/type/Text.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> TextFactory::ReadResource(std::shared_ptr<ResourceMgr> resourceMgr,
                                                    std::shared_ptr<ResourceInitData> initData,
                                                    std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<Text>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
        case 0:
	    factory = std::make_shared<TextFactoryV0>();
	    break;
        default:
            // VERSION NOT SUPPORTED
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Text with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::TextFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
    std::shared_ptr<Text> text = std::static_pointer_cast<Text>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, text);

    uint32_t msgCount = reader->ReadUInt32();
    text->messages.reserve(msgCount);

    for (uint32_t i = 0; i < msgCount; i++) {
	MessageEntry entry;
	entry.id = reader->ReadUInt16();
	entry.textboxType = reader->ReadUByte();
	entry.textboxYPos = reader->ReadUByte();
	entry.msg = reader->ReadString();

	text->messages.push_back(entry);
    }
}
} // namespace Ship
