#include "2s2h/resource/importer/scenecommand/SetRoomBehaviorFactory.h"
#include "2s2h/resource/type/scenecommand/SetRoomBehavior.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetRoomBehaviorMMFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                       std::shared_ptr<Ship::BinaryReader> reader) {
    auto setRoomBehavior = std::make_shared<SetRoomBehaviorMM>(initData);

    ReadCommandId(setRoomBehavior, reader);

    setRoomBehavior->roomBehavior.gameplayFlags = reader->ReadInt8();
    setRoomBehavior->roomBehavior.currRoomUnk2 = reader->ReadInt8();
    setRoomBehavior->roomBehavior.currRoomUnk5 = reader->ReadInt8();
    setRoomBehavior->roomBehavior.msgCtxUnk = reader->ReadInt8();
    setRoomBehavior->roomBehavior.enablePointLights = reader->ReadInt8();
    setRoomBehavior->roomBehavior.kankyoContextUnkE2 = reader->ReadInt8();

    return setRoomBehavior;
}
} // namespace SOH
