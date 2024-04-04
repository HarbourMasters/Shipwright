#include "soh/resource/importer/RawJsonFactory.h"
#include "soh/resource/type/RawJson.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> ResourceFactoryBinaryRawJsonV0::ReadResource(std::shared_ptr<LUS::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto rawJson = std::make_shared<RawJson>(file->InitData);
    auto reader = std::get<std::shared_ptr<LUS::BinaryReader>>(file->Reader);

    rawJson->DataSize = file->Buffer->size();
    rawJson->Data = nlohmann::json::parse(reader->ReadCString(), nullptr, true, true);

    return rawJson;
}
} // namespace SOH
