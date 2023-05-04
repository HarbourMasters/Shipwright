#include "soh/resource/importer/PlayerAnimationFactory.h"
#include "soh/resource/type/PlayerAnimation.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> PlayerAnimationFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                               std::shared_ptr<ResourceInitData> initData,
                                                               std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<PlayerAnimation>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<PlayerAnimationFactoryV0>();
	break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load PlayerAnimation with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::PlayerAnimationFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                     std::shared_ptr<Resource> resource)
{
    std::shared_ptr<PlayerAnimation> playerAnimation = std::static_pointer_cast<PlayerAnimation>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, playerAnimation);

    uint32_t numEntries = reader->ReadUInt32();
    playerAnimation->limbRotData.reserve(numEntries);

    for (uint32_t i = 0; i < numEntries; i++) {
        playerAnimation->limbRotData.push_back(reader->ReadInt16());
    }
}
} // namespace Ship
