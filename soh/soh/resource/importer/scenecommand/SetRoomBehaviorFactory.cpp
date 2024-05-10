#include "soh/resource/importer/scenecommand/SetRoomBehaviorFactory.h"
#include "soh/resource/type/scenecommand/SetRoomBehavior.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetRoomBehaviorFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setRoomBehavior = std::make_shared<SetRoomBehavior>(initData);

    ReadCommandId(setRoomBehavior, reader);
	
    setRoomBehavior->roomBehavior.gameplayFlags = reader->ReadInt8();
    setRoomBehavior->roomBehavior.gameplayFlags2 = reader->ReadInt32();

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogRoomBehaviorAsXML(setRoomBehavior);
    }

    return setRoomBehavior;
}

std::shared_ptr<Ship::IResource> SetRoomBehaviorFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setRoomBehavior = std::make_shared<SetRoomBehavior>(initData);

    setRoomBehavior->cmdId = SceneCommandID::SetRoomBehavior;

    setRoomBehavior->roomBehavior.gameplayFlags = reader->IntAttribute("GameplayFlags1");
    setRoomBehavior->roomBehavior.gameplayFlags2 = reader->IntAttribute("GameplayFlags2");

    return setRoomBehavior;
}
} // namespace SOH
