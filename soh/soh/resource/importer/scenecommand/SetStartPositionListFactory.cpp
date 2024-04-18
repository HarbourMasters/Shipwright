#include "soh/resource/importer/scenecommand/SetStartPositionListFactory.h"
#include "soh/resource/type/scenecommand/SetStartPositionList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetStartPositionListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                    std::shared_ptr<LUS::BinaryReader> reader) {
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

    //LogStartPositionListAsXML(setStartPositionList);

    return setStartPositionList;
}

std::shared_ptr<LUS::IResource> SetStartPositionListFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
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

void LogStartPositionListAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetStartPositionList> setStartPositionList = std::static_pointer_cast<SetStartPositionList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetStartPositionList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setStartPositionList->numStartPositions; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("StartPositionEntry");
        entry->SetAttribute("Id", setStartPositionList->startPositions[i].id);
        entry->SetAttribute("PosX", setStartPositionList->startPositions[i].pos.x);
        entry->SetAttribute("PosY", setStartPositionList->startPositions[i].pos.y);
        entry->SetAttribute("PosZ", setStartPositionList->startPositions[i].pos.z);
        entry->SetAttribute("RotX", setStartPositionList->startPositions[i].rot.x);
        entry->SetAttribute("RotY", setStartPositionList->startPositions[i].rot.y);
        entry->SetAttribute("RotZ", setStartPositionList->startPositions[i].rot.z);
        entry->SetAttribute("Params", setStartPositionList->startPositions[i].params);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
