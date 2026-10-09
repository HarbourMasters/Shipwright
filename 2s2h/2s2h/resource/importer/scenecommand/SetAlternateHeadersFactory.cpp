#include "2s2h/resource/importer/scenecommand/SetAlternateHeadersFactory.h"
#include "2s2h/resource/type/scenecommand/SetAlternateHeaders.h"
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>

namespace SOH {
std::shared_ptr<Ship::IResource>
SetAlternateHeadersFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                         std::shared_ptr<Ship::BinaryReader> reader) {
    auto setAlternateHeaders = std::make_shared<SetAlternateHeaders>(initData);

    ReadCommandId(setAlternateHeaders, reader);

    setAlternateHeaders->numHeaders = reader->ReadUInt32();
    setAlternateHeaders->headers.reserve(setAlternateHeaders->numHeaders);
    for (uint32_t i = 0; i < setAlternateHeaders->numHeaders; i++) {
        auto headerName = reader->ReadString();
        if (!headerName.empty()) {
            setAlternateHeaders->headers.push_back(std::static_pointer_cast<Scene>(
                Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(headerName.c_str())));
        } else {
            setAlternateHeaders->headers.push_back(nullptr);
        }
    }

    return setAlternateHeaders;
}
} // namespace SOH
