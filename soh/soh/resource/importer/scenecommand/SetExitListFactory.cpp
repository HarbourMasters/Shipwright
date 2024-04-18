#include "soh/resource/importer/scenecommand/SetExitListFactory.h"
#include "soh/resource/type/scenecommand/SetExitList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetExitListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setExitList = std::make_shared<SetExitList>( initData);

    ReadCommandId(setExitList, reader);
	
    setExitList->numExits = reader->ReadUInt32();
    setExitList->exits.reserve(setExitList->numExits);
    for (uint32_t i = 0; i < setExitList->numExits; i++) {
        setExitList->exits.push_back(reader->ReadUInt16());
    }

    //LogExitListAsXML(setExitList);

    return setExitList;
}

std::shared_ptr<LUS::IResource> SetExitListFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
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

void LogExitListAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetExitList> setExitList = std::static_pointer_cast<SetExitList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetExitList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setExitList->numExits; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("ExitEntry");
        entry->SetAttribute("Id", setExitList->exits[i]);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
