#include "2s2h/resource/importer/scenecommand/EndMarkerFactory.h"
#include "2s2h/resource/type/scenecommand/EndMarker.h"

namespace SOH {
std::shared_ptr<Ship::IResource> EndMarkerFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                std::shared_ptr<Ship::BinaryReader> reader) {
    auto endMarker = std::make_shared<EndMarker>(initData);

    ReadCommandId(endMarker, reader);

    return endMarker;
}
} // namespace SOH
