#include "soh/resource/importer/scenecommand/SetStartPositionListFactory.h"
#include "soh/resource/type/scenecommand/SetStartPositionList.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetStartPositionListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                    std::shared_ptr<Ship::BinaryReader> reader) {
    auto setStartPositionList = std::make_shared<SetStartPositionList>(initData);

    ReadCommandId(setStartPositionList, reader);
	
    setStartPositionList->numStartPositions = reader->ReadUInt32();
    setStartPositionList->startPositions.reserve(setStartPositionList->numStartPositions);
    for (uint32_t i = 0; i < setStartPositionList->numStartPositions; i++) {
        ActorEntry entry;

        entry.id = reader->ReadUInt16();
        entry.pos.x = reader->ReadInt16();
        entry.pos.y = reader->ReadInt16();
        entry.pos.z = reader->ReadInt16();
        entry.rot.x = reader->ReadInt16();
        entry.rot.y = reader->ReadInt16();
        entry.rot.z = reader->ReadInt16();
        entry.params = reader->ReadUInt16();

        setStartPositionList->startPositions.push_back(entry);
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogStartPositionListAsXML(setStartPositionList);
    }

    return setStartPositionList;
}

std::shared_ptr<Ship::IResource> SetStartPositionListFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setStartPositionList = std::make_shared<SetStartPositionList>(initData);

    setStartPositionList->cmdId = SceneCommandID::SetStartPositionList;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "StartPositionEntry") {
            ActorEntry entry;
            entry.id = child->IntAttribute("Id");
            entry.pos.x = child->IntAttribute("PosX");
            entry.pos.y = child->IntAttribute("PosY");
            entry.pos.z = child->IntAttribute("PosZ");
            entry.rot.x = child->IntAttribute("RotX");
            entry.rot.y = child->IntAttribute("RotY");
            entry.rot.z = child->IntAttribute("RotZ");
            entry.params = child->IntAttribute("Params");
            setStartPositionList->startPositions.push_back(entry);
        }

        child = child->NextSiblingElement();
    }

    setStartPositionList->numStartPositions = setStartPositionList->startPositions.size();

    return setStartPositionList;
}
} // namespace SOH
