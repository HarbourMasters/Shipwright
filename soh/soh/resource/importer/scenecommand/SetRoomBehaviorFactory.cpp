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

    return setRoomBehavior;
}
} // namespace SOH
