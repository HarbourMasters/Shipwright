#include "soh/resource/importer/scenecommand/SetObjectListFactory.h"
#include "soh/resource/type/scenecommand/SetObjectList.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetObjectListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setObjectList = std::make_shared<SetObjectList>(initData);

    ReadCommandId(setObjectList, reader);
	
    setObjectList->numObjects = reader->ReadUInt32();
    setObjectList->objects.reserve(setObjectList->numObjects);
    for (uint32_t i = 0; i < setObjectList->numObjects; i++) {
        setObjectList->objects.push_back(reader->ReadUInt16());
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogObjectListAsXML(setObjectList);
    }

    return setObjectList;
}

std::shared_ptr<Ship::IResource> SetObjectListFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setObjectList = std::make_shared<SetObjectList>(initData);

    setObjectList->cmdId = SceneCommandID::SetObjectList;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "ObjectEntry") {
            setObjectList->objects.push_back(child->IntAttribute("Id"));
        }

        child = child->NextSiblingElement();
    }

    setObjectList->numObjects = setObjectList->objects.size();

    return setObjectList;
}
} // namespace SOH
