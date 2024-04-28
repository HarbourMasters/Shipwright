#include "soh/resource/importer/scenecommand/SetAlternateHeadersFactory.h"
#include "soh/resource/type/scenecommand/SetAlternateHeaders.h"
#include "spdlog/spdlog.h"
#include "libultraship/libultraship.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetAlternateHeadersFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   std::shared_ptr<Ship::BinaryReader> reader) {
    auto setAlternateHeaders = std::make_shared<SetAlternateHeaders>(initData);

    ReadCommandId(setAlternateHeaders, reader);
	
    setAlternateHeaders->numHeaders = reader->ReadUInt32();
    setAlternateHeaders->headers.reserve(setAlternateHeaders->numHeaders);
    for (uint32_t i = 0; i < setAlternateHeaders->numHeaders; i++) {
        auto headerName = reader->ReadString();
        if (!headerName.empty()) {
            setAlternateHeaders->headers.push_back(std::static_pointer_cast<Scene>(Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(headerName.c_str())));
        } else {
            setAlternateHeaders->headers.push_back(nullptr);
        }
    }

    return setAlternateHeaders;
}
} // namespace SOH
