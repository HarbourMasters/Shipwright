#include "soh/resource/importer/scenecommand/SetCollisionHeaderFactory.h"
#include "soh/resource/type/scenecommand/SetCollisionHeader.h"
#include "libultraship/bridge.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetCollisionHeaderFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                                  std::shared_ptr<ResourceInitData> initData,
                                                                  std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetCollisionHeader>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<SetCollisionHeaderFactoryV0>();
	break;
    }

    if (factory == nullptr) {
	SPDLOG_ERROR("Failed to load SetCollisionHeader with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::SetCollisionHeaderFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
    std::shared_ptr<SetCollisionHeader> setCollisionHeader = std::static_pointer_cast<SetCollisionHeader>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setCollisionHeader);

    ReadCommandId(setCollisionHeader, reader);
    
    setCollisionHeader->fileName = reader->ReadString();
    setCollisionHeader->collisionHeader = std::static_pointer_cast<CollisionHeader>(LoadResource(setCollisionHeader->fileName.c_str(), true));
}

} // namespace Ship
