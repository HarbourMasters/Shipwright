#include "soh/resource/importer/scenecommand/SetEntranceListFactory.h"
#include "soh/resource/type/scenecommand/SetEntranceList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetEntranceListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
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

    //LogEntranceListAsXML(setEntranceList);

    return setEntranceList;
}

std::shared_ptr<LUS::IResource> SetEntranceListFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
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

void LogEntranceListAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetEntranceList> setEntranceList = std::static_pointer_cast<SetEntranceList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetEntranceList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setEntranceList->numEntrances; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("EntranceEntry");
        entry->SetAttribute("Spawn", setEntranceList->entrances[i].spawn);
        entry->SetAttribute("Room", setEntranceList->entrances[i].room);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
