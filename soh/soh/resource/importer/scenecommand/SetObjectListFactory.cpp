#include "soh/resource/importer/scenecommand/SetObjectListFactory.h"
#include "soh/resource/type/scenecommand/SetObjectList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetObjectListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setObjectList = std::make_shared<SetObjectList>(initData);

    ReadCommandId(setObjectList, reader);
	
    setObjectList->numObjects = reader->ReadUInt32();
    setObjectList->objects.reserve(setObjectList->numObjects);
    for (uint32_t i = 0; i < setObjectList->numObjects; i++) {
        setObjectList->objects.push_back(reader->ReadUInt16());
    }

    //LogObjectListAsXML(setObjectList);

    return setObjectList;
}

std::shared_ptr<LUS::IResource> SetObjectListFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
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

void LogObjectListAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetObjectList> setObjectList = std::static_pointer_cast<SetObjectList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetObjectList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setObjectList->numObjects; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("ObjectEntry");
        entry->SetAttribute("Id", setObjectList->objects[i]);
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
