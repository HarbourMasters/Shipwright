#include "soh/resource/importer/scenecommand/EndMarkerFactory.h"
#include "soh/resource/type/scenecommand/EndMarker.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<ShipDK::IResource>
EndMarkerFactory::ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData, std::shared_ptr<ShipDK::BinaryReader> reader) {
    auto endMarker = std::make_shared<EndMarker>(initData);

    ReadCommandId(endMarker, reader);

    return endMarker;
}
} // namespace SOH
