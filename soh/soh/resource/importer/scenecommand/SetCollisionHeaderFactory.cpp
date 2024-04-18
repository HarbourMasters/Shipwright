#include "soh/resource/importer/scenecommand/SetCollisionHeaderFactory.h"
#include "soh/resource/type/scenecommand/SetCollisionHeader.h"
#include "libultraship/libultraship.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetCollisionHeaderFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                  std::shared_ptr<LUS::BinaryReader> reader) {
    auto setCollisionHeader = std::make_shared<SetCollisionHeader>(initData);

    ReadCommandId(setCollisionHeader, reader);
    
    setCollisionHeader->fileName = reader->ReadString();
    setCollisionHeader->collisionHeader = std::static_pointer_cast<CollisionHeader>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(setCollisionHeader->fileName.c_str()));

    //LogSetCollisionHeaderAsXML(setCollisionHeader);

    return setCollisionHeader;
}

std::shared_ptr<LUS::IResource> SetCollisionHeaderFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setCollisionHeader = std::make_shared<SetCollisionHeader>(initData);

    setCollisionHeader->cmdId = SceneCommandID::SetCollisionHeader;

    setCollisionHeader->fileName = reader->Attribute("FileName");
    setCollisionHeader->collisionHeader = std::static_pointer_cast<CollisionHeader>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(setCollisionHeader->fileName.c_str()));

    return setCollisionHeader;
}

void LogSetCollisionHeaderAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetCollisionHeader> setCollisionHeader = std::static_pointer_cast<SetCollisionHeader>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetCollisionHeader");
    doc.InsertFirstChild(root);

    root->SetAttribute("FileName", setCollisionHeader->fileName.c_str());

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
