#include "2s2h/resource/importer/scenecommand/SetExitListFactory.h"
#include "2s2h/resource/type/scenecommand/SetExitList.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetExitListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                  std::shared_ptr<Ship::BinaryReader> reader) {
    auto setExitList = std::make_shared<SetExitList>(initData);

    ReadCommandId(setExitList, reader);

    setExitList->numExits = reader->ReadUInt32();
    setExitList->exits.reserve(setExitList->numExits);
    for (uint32_t i = 0; i < setExitList->numExits; i++) {
        setExitList->exits.push_back(reader->ReadUInt16());
    }
    return setExitList;
}
} // namespace SOH
