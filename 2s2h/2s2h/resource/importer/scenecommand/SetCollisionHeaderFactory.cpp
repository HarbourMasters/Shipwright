#include "2s2h/resource/importer/scenecommand/SetCollisionHeaderFactory.h"
#include "2s2h/resource/type/scenecommand/SetCollisionHeader.h"
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>

namespace SOH {
std::shared_ptr<Ship::IResource>
SetCollisionHeaderFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                        std::shared_ptr<Ship::BinaryReader> reader) {
    auto setCollisionHeader = std::make_shared<SetCollisionHeader>(initData);

    ReadCommandId(setCollisionHeader, reader);

    setCollisionHeader->fileName = reader->ReadString();
    setCollisionHeader->collisionHeader = std::static_pointer_cast<CollisionHeader>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(
            setCollisionHeader->fileName.c_str()));

    return setCollisionHeader;
}
} // namespace SOH
