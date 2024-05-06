#include "soh/resource/importer/scenecommand/SetActorListFactory.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetActorListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setActorList = std::make_shared<SetActorList>(initData);

    ReadCommandId(setActorList, reader);
	
    setActorList->numActors = reader->ReadUInt32();
    setActorList->actorList.reserve(setActorList->numActors);
    for (uint32_t i = 0; i < setActorList->numActors; i++) {
        ActorEntry entry;

        entry.id = reader->ReadUInt16();
        entry.pos.x = reader->ReadInt16();
        entry.pos.y = reader->ReadInt16();
        entry.pos.z = reader->ReadInt16();
        entry.rot.x = reader->ReadInt16();
        entry.rot.y = reader->ReadInt16();
        entry.rot.z = reader->ReadInt16();
        entry.params = reader->ReadUInt16();

        setActorList->actorList.push_back(entry);
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogActorListAsXML(setActorList);
    }

    return setActorList;
}

std::shared_ptr<Ship::IResource> SetActorListFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setActorList = std::make_shared<SetActorList>(initData);

    setActorList->cmdId = SceneCommandID::SetActorList;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "ActorEntry") {
            ActorEntry entry;
            entry.id = child->IntAttribute("Id");
            entry.pos.x = child->IntAttribute("PosX");
            entry.pos.y = child->IntAttribute("PosY");
            entry.pos.z = child->IntAttribute("PosZ");
            entry.rot.x = child->IntAttribute("RotX");
            entry.rot.y = child->IntAttribute("RotY");
            entry.rot.z = child->IntAttribute("RotZ");
            entry.params = child->IntAttribute("Params");
            setActorList->actorList.push_back(entry);
        }

        child = child->NextSiblingElement();
    }

    setActorList->numActors = setActorList->actorList.size();

    return setActorList;
}
} // namespace SOH
