#include "soh/resource/importer/scenecommand/SetSpecialObjectsFactory.h"
#include "soh/resource/type/scenecommand/SetSpecialObjects.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetSpecialObjectsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                 std::shared_ptr<Ship::BinaryReader> reader) {
    auto setSpecialObjects = std::make_shared<SetSpecialObjects>(initData);

    ReadCommandId(setSpecialObjects, reader);
    
    setSpecialObjects->specialObjects.elfMessage = reader->ReadInt8();
    setSpecialObjects->specialObjects.globalObject = reader->ReadInt16();

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogSpecialObjectsAsXML(setSpecialObjects);
    }

    return setSpecialObjects;
}

std::shared_ptr<Ship::IResource> SetSpecialObjectsFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setSpecialObjects = std::make_shared<SetSpecialObjects>(initData);

    setSpecialObjects->cmdId = SceneCommandID::SetSpecialObjects;

    setSpecialObjects->specialObjects.elfMessage = reader->IntAttribute("ElfMessage");
    setSpecialObjects->specialObjects.globalObject = reader->IntAttribute("GlobalObject");

    return setSpecialObjects;
}
} // namespace SOH
