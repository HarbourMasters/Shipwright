#include "soh/resource/importer/BackgroundFactory.h"
#include "soh/resource/type/Background.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource> ResourceFactoryBinaryBackgroundV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto background = std::make_shared<Background>(file->InitData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    uint32_t dataSize = reader->ReadUInt32();

    background->Data.reserve(dataSize);

    for (uint32_t i = 0; i < dataSize; i++) {
        background->Data.push_back(reader->ReadUByte());
    }

    return background;
}
} // namespace SOH
