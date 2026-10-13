#include "2s2h/resource/importer/scenecommand/SetObjectListFactory.h"
#include "2s2h/resource/type/scenecommand/SetObjectList.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetObjectListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                    std::shared_ptr<Ship::BinaryReader> reader) {
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
