#include "soh/resource/importer/scenecommand/SetTransitionActorListFactory.h"
#include "soh/resource/type/scenecommand/SetTransitionActorList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetTransitionActorListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                      std::shared_ptr<LUS::BinaryReader> reader) {
    auto setTransitionActorList = std::make_shared<SetTransitionActorList>(initData);
    
    ReadCommandId(setTransitionActorList, reader);
	
    setTransitionActorList->numTransitionActors = reader->ReadUInt32();
    setTransitionActorList->transitionActorList.reserve(setTransitionActorList->numTransitionActors);
    for (uint32_t i = 0; i < setTransitionActorList->numTransitionActors; i++) {
        TransitionActorEntry entry;

        entry.sides[0].room = reader->ReadUByte();
        entry.sides[0].effects = reader->ReadUByte();
        entry.sides[1].room = reader->ReadUByte();
        entry.sides[1].effects = reader->ReadUByte();
        entry.id = reader->ReadInt16();
        entry.pos.x = reader->ReadInt16();
        entry.pos.y = reader->ReadInt16();
        entry.pos.z = reader->ReadInt16();
        entry.rotY = reader->ReadInt16();
        entry.params = reader->ReadUInt16();

        setTransitionActorList->transitionActorList.push_back(entry);
    }

    //LogTransitionActorListAsXML(setTransitionActorList);

    return setTransitionActorList;
}

std::shared_ptr<LUS::IResource> SetTransitionActorListFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setTransitionActorList = std::make_shared<SetTransitionActorList>(initData);

    setTransitionActorList->cmdId = SceneCommandID::SetTransitionActorList;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "TransitionActorEntry") {
            TransitionActorEntry entry;
            entry.sides[0].room = child->IntAttribute("FrontSideRoom");
            entry.sides[0].effects = child->IntAttribute("FrontSideEffects");
            entry.sides[1].room = child->IntAttribute("BackSideRoom");
            entry.sides[1].effects = child->IntAttribute("BackSideEffects");
            entry.id = child->IntAttribute("Id");
            entry.pos.x = child->IntAttribute("PosX");
            entry.pos.y = child->IntAttribute("PosY");
            entry.pos.z = child->IntAttribute("PosZ");
            entry.rotY = child->IntAttribute("RotY");
            entry.params = child->IntAttribute("Params");
            setTransitionActorList->transitionActorList.push_back(entry);
        }

        child = child->NextSiblingElement();
    }

    setTransitionActorList->numTransitionActors = setTransitionActorList->transitionActorList.size();

    return setTransitionActorList;
}

void LogTransitionActorListAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetTransitionActorList> setTransitionActorList = std::static_pointer_cast<SetTransitionActorList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetTransitionActorList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setTransitionActorList->numTransitionActors; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("TransitionActorEntry");
        entry->SetAttribute("FrontSideRoom", setTransitionActorList->transitionActorList[i].sides[0].room);
        entry->SetAttribute("FrontSideEffects", setTransitionActorList->transitionActorList[i].sides[0].effects);
        entry->SetAttribute("BackSideRoom", setTransitionActorList->transitionActorList[i].sides[1].room);
        entry->SetAttribute("BackSideEffects", setTransitionActorList->transitionActorList[i].sides[1].effects);
        entry->SetAttribute("Id", setTransitionActorList->transitionActorList[i].id);
        entry->SetAttribute("PosX", setTransitionActorList->transitionActorList[i].pos.x);
        entry->SetAttribute("PosY", setTransitionActorList->transitionActorList[i].pos.y);
        entry->SetAttribute("PosZ", setTransitionActorList->transitionActorList[i].pos.z);
        entry->SetAttribute("RotY", setTransitionActorList->transitionActorList[i].rotY);
        entry->SetAttribute("Params", setTransitionActorList->transitionActorList[i].params);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
