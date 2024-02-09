#include "soh/resource/importer/PlayerAnimationFactory.h"
#include "soh/resource/type/PlayerAnimation.h"
#include "spdlog/spdlog.h"

std::shared_ptr<LUS::IResource> ResourceFactoryBinaryPlayerAnimationV0::ReadResource(std::shared_ptr<LUS::File> file) {
    if (file->InitData->Format != RESOURCE_FORMAT_BINARY) {
        SPDLOG_ERROR("resource file format does not match factory format.");
        return nullptr;
    }

    if (file->Reader == nullptr) {
        SPDLOG_ERROR("Failed to load resource: File has Reader ({} - {})", file->InitData->Type,
                        file->InitData->Path);
        return nullptr;
    }

    auto playerAnimation = std::make_shared<PlayerAnimation>(file->InitData);

    uint32_t numEntries = file->Reader->ReadUInt32();
    playerAnimation->limbRotData.reserve(numEntries);

    for (uint32_t i = 0; i < numEntries; i++) {
        playerAnimation->limbRotData.push_back(file->Reader->ReadInt16());
    }

    return playerAnimation;
};
