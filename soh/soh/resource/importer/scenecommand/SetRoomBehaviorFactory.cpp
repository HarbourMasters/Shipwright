#include "soh/resource/importer/scenecommand/SetRoomBehaviorFactory.h"
#include "soh/resource/type/scenecommand/SetRoomBehavior.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetRoomBehaviorFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setRoomBehavior = std::make_shared<SetRoomBehavior>(initData);

    ReadCommandId(setRoomBehavior, reader);
	
    setRoomBehavior->roomBehavior.gameplayFlags = reader->ReadInt8();
    setRoomBehavior->roomBehavior.gameplayFlags2 = reader->ReadInt32();

    //LogRoomBehaviorAsXML(setRoomBehavior);

    return setRoomBehavior;
}

std::shared_ptr<LUS::IResource> SetRoomBehaviorFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setRoomBehavior = std::make_shared<SetRoomBehavior>(initData);

    setRoomBehavior->cmdId = SceneCommandID::SetRoomBehavior;

    setRoomBehavior->roomBehavior.gameplayFlags = reader->IntAttribute("GameplayFlags1");
    setRoomBehavior->roomBehavior.gameplayFlags2 = reader->IntAttribute("GameplayFlags2");

    return setRoomBehavior;
}

void LogRoomBehaviorAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetRoomBehavior> setRoomBehavior = std::static_pointer_cast<SetRoomBehavior>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetRoomBehavior");
    doc.InsertFirstChild(root);

    root->SetAttribute("GameplayFlags1", setRoomBehavior->roomBehavior.gameplayFlags);
    root->SetAttribute("GameplayFlags2", setRoomBehavior->roomBehavior.gameplayFlags2);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
