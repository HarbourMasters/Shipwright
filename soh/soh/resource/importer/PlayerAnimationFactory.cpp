#include "soh/resource/importer/PlayerAnimationFactory.h"
#include "soh/resource/type/PlayerAnimation.h"
#include "spdlog/spdlog.h"

namespace LUS {
std::shared_ptr<IResource>
PlayerAnimationFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<PlayerAnimation>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	factory = std::make_shared<PlayerAnimationFactoryV0>();
	break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load PlayerAnimation with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::PlayerAnimationFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                     std::shared_ptr<IResource> resource)
{
    std::shared_ptr<PlayerAnimation> playerAnimation = std::static_pointer_cast<PlayerAnimation>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, playerAnimation);

    uint32_t numEntries = reader->ReadUInt32();
    playerAnimation->limbRotData.reserve(numEntries);

    for (uint32_t i = 0; i < numEntries; i++) {
        playerAnimation->limbRotData.push_back(reader->ReadInt16());
    }
}
} // namespace LUS
