#include "soh/resource/importer/scenecommand/EndMarkerFactory.h"
#include "soh/resource/type/scenecommand/EndMarker.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
EndMarkerFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto endMarker = std::make_shared<EndMarker>(initData);

    ReadCommandId(endMarker, reader);

    if (CVarGetInteger("gDebugResourceLogging", 0)) {
        LogEndMarkerAsXML(endMarker);
    }

    return endMarker;
}

std::shared_ptr<LUS::IResource> EndMarkerFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto endMarker = std::make_shared<EndMarker>(initData);

    endMarker->cmdId = SceneCommandID::EndMarker;

    return endMarker;
}
} // namespace SOH
