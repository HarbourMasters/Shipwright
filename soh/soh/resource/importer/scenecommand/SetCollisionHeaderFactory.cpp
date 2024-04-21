#include "soh/resource/importer/scenecommand/SetCollisionHeaderFactory.h"
#include "soh/resource/type/scenecommand/SetCollisionHeader.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "libultraship/libultraship.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetCollisionHeaderFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                  std::shared_ptr<LUS::BinaryReader> reader) {
    auto setCollisionHeader = std::make_shared<SetCollisionHeader>(initData);

    ReadCommandId(setCollisionHeader, reader);
    
    setCollisionHeader->fileName = reader->ReadString();
    setCollisionHeader->collisionHeader = std::static_pointer_cast<CollisionHeader>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(setCollisionHeader->fileName.c_str()));

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogSetCollisionHeaderAsXML(setCollisionHeader);
    }

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
} // namespace SOH
