#include "soh/resource/importer/PlayerAnimationFactory.h"
#include "soh/resource/type/PlayerAnimation.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> ResourceFactoryBinaryPlayerAnimationV0::ReadResource(std::shared_ptr<LUS::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto playerAnimation = std::make_shared<PlayerAnimation>(file->InitData);
    auto reader = std::get<std::shared_ptr<LUS::BinaryReader>>(file->Reader);

    uint32_t numEntries = reader->ReadUInt32();
    playerAnimation->limbRotData.reserve(numEntries);

    for (uint32_t i = 0; i < numEntries; i++) {
        playerAnimation->limbRotData.push_back(reader->ReadInt16());
    }

    return playerAnimation;
};
} // namespace SOH
