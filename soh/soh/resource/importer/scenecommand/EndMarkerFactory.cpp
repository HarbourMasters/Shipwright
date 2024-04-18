#include "soh/resource/importer/scenecommand/EndMarkerFactory.h"
#include "soh/resource/type/scenecommand/EndMarker.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
EndMarkerFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto endMarker = std::make_shared<EndMarker>(initData);

    ReadCommandId(endMarker, reader);

    //LogEndMarkerAsXML(endMarker);

    return endMarker;
}

std::shared_ptr<LUS::IResource> EndMarkerFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto endMarker = std::make_shared<EndMarker>(initData);

    endMarker->cmdId = SceneCommandID::EndMarker;

    return endMarker;
}

void LogEndMarkerAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<EndMarker> endMarker = std::static_pointer_cast<EndMarker>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("EndMarker");
    doc.InsertFirstChild(root);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
