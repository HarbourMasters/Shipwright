#include "soh/resource/importer/scenecommand/SetObjectListFactory.h"
#include "soh/resource/type/scenecommand/SetObjectList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetObjectListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
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
