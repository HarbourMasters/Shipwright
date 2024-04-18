#include "soh/resource/importer/scenecommand/SetActorListFactory.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetActorListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
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

    //LogActorListAsXML(setActorList);

    return setActorList;
}

std::shared_ptr<LUS::IResource> SetActorListFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
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

void LogActorListAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetActorList> setActorList = std::static_pointer_cast<SetActorList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetActorList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setActorList->numActors; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("ActorEntry");
        entry->SetAttribute("Id", setActorList->actorList[i].id);
        entry->SetAttribute("PosX", setActorList->actorList[i].pos.x);
        entry->SetAttribute("PosY", setActorList->actorList[i].pos.y);
        entry->SetAttribute("PosZ", setActorList->actorList[i].pos.z);
        entry->SetAttribute("RotX", setActorList->actorList[i].rot.x);
        entry->SetAttribute("RotY", setActorList->actorList[i].rot.y);
        entry->SetAttribute("RotZ", setActorList->actorList[i].rot.z);
        entry->SetAttribute("Params", setActorList->actorList[i].params);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
