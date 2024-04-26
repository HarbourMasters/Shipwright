#include "soh/resource/importer/scenecommand/SetExitListFactory.h"
#include "soh/resource/type/scenecommand/SetExitList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<ShipDK::IResource>
SetExitListFactory::ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData, std::shared_ptr<ShipDK::BinaryReader> reader) {
    auto setExitList = std::make_shared<SetExitList>( initData);

    ReadCommandId(setExitList, reader);
	
    setExitList->numExits = reader->ReadUInt32();
    setExitList->exits.reserve(setExitList->numExits);
    for (uint32_t i = 0; i < setExitList->numExits; i++) {
        setExitList->exits.push_back(reader->ReadUInt16());
    }

    return setExitList;
}
} // namespace SOH
