#include "soh/resource/importer/scenecommand/SetSpecialObjectsFactory.h"
#include "soh/resource/type/scenecommand/SetSpecialObjects.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetSpecialObjectsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                 std::shared_ptr<LUS::BinaryReader> reader) {
    auto setSpecialObjects = std::make_shared<SetSpecialObjects>(initData);

    ReadCommandId(setSpecialObjects, reader);
    
    setSpecialObjects->specialObjects.elfMessage = reader->ReadInt8();
    setSpecialObjects->specialObjects.globalObject = reader->ReadInt16();

    //LogSpecialObjectsAsXML(setSpecialObjects);

    return setSpecialObjects;
}

std::shared_ptr<LUS::IResource> SetSpecialObjectsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setSpecialObjects = std::make_shared<SetSpecialObjects>(initData);

    setSpecialObjects->cmdId = SceneCommandID::SetSpecialObjects;

    setSpecialObjects->specialObjects.elfMessage = reader->IntAttribute("ElfMessage");
    setSpecialObjects->specialObjects.globalObject = reader->IntAttribute("GlobalObject");

    return setSpecialObjects;
}

void LogSpecialObjectsAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetSpecialObjects> setSpecialObjects = std::static_pointer_cast<SetSpecialObjects>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetSpecialObjects");
    doc.InsertFirstChild(root);

    root->SetAttribute("ElfMessage", setSpecialObjects->specialObjects.elfMessage);
    root->SetAttribute("GlobalObject", setSpecialObjects->specialObjects.globalObject);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
