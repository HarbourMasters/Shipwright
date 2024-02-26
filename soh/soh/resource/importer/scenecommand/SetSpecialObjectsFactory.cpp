#include "soh/resource/importer/scenecommand/SetSpecialObjectsFactory.h"
#include "soh/resource/type/scenecommand/SetSpecialObjects.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetSpecialObjectsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                 std::shared_ptr<LUS::BinaryReader> reader) {
    auto setSpecialObjects = std::make_shared<SetSpecialObjects>(initData);

    ReadCommandId(setSpecialObjects, reader);
    
    setSpecialObjects->specialObjects.elfMessage = reader->ReadInt8();
    setSpecialObjects->specialObjects.globalObject = reader->ReadInt16();

    return setSpecialObjects;
}
} // namespace SOH
