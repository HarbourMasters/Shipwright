#include "soh/resource/importer/scenecommand/SetEntranceListFactory.h"
#include "soh/resource/type/scenecommand/SetEntranceList.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetEntranceListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setEntranceList = std::make_shared<SetEntranceList>(initData);

    ReadCommandId(setEntranceList, reader);
	
    setEntranceList->numEntrances = reader->ReadUInt32();
    setEntranceList->entrances.reserve(setEntranceList->numEntrances);
    for (uint32_t i = 0; i < setEntranceList->numEntrances; i++) {
	EntranceEntry entranceEntry;

	entranceEntry.spawn = reader->ReadInt8();
	entranceEntry.room = reader->ReadInt8();
		
        setEntranceList->entrances.push_back(entranceEntry);
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogEntranceListAsXML(setEntranceList);
    }

    return setEntranceList;
}

std::shared_ptr<Ship::IResource> SetEntranceListFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setEntranceList = std::make_shared<SetEntranceList>(initData);

    setEntranceList->cmdId = SceneCommandID::SetEntranceList;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "EntranceEntry") {
            EntranceEntry entry;
            entry.spawn = child->IntAttribute("Spawn");
            entry.room = child->IntAttribute("Room");
            setEntranceList->entrances.push_back(entry);
        }

        child = child->NextSiblingElement();
    }

    setEntranceList->numEntrances = setEntranceList->entrances.size();

    return setEntranceList;
}
} // namespace SOH
