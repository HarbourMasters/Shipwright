#include "soh/resource/importer/scenecommand/SetObjectListFactory.h"
#include "soh/resource/type/scenecommand/SetObjectList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<ShipDK::IResource>
SetObjectListFactory::ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData, std::shared_ptr<ShipDK::BinaryReader> reader) {
    auto setObjectList = std::make_shared<SetObjectList>(initData);

    ReadCommandId(setObjectList, reader);
	
    setObjectList->numObjects = reader->ReadUInt32();
    setObjectList->objects.reserve(setObjectList->numObjects);
    for (uint32_t i = 0; i < setObjectList->numObjects; i++) {
        setObjectList->objects.push_back(reader->ReadUInt16());
    }

    return setObjectList;
}
} // namespace SOH
