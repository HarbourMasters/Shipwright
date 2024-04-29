#include "soh/resource/importer/scenecommand/SetExitListFactory.h"
#include "soh/resource/type/scenecommand/SetExitList.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetExitListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setExitList = std::make_shared<SetExitList>( initData);

    ReadCommandId(setExitList, reader);
	
    setExitList->numExits = reader->ReadUInt32();
    setExitList->exits.reserve(setExitList->numExits);
    for (uint32_t i = 0; i < setExitList->numExits; i++) {
        setExitList->exits.push_back(reader->ReadUInt16());
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogExitListAsXML(setExitList);
    }

    return setExitList;
}

std::shared_ptr<Ship::IResource> SetExitListFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setExitList = std::make_shared<SetExitList>(initData);

    setExitList->cmdId = SceneCommandID::SetExitList;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "ExitEntry") {
            setExitList->exits.push_back(child->IntAttribute("Id"));
        }

        child = child->NextSiblingElement();
    }

    setExitList->numExits = setExitList->exits.size();

    return setExitList;
}
} // namespace SOH
