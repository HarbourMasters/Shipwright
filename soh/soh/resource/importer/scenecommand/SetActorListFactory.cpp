#include "soh/resource/importer/scenecommand/SetActorListFactory.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetActorListFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                            std::shared_ptr<ResourceInitData> initData,
                                                            std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetActorList>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<SetActorListFactoryV0>();
	break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load SetActorList with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::SetActorListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
    std::shared_ptr<SetActorList> setActorList = std::static_pointer_cast<SetActorList>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setActorList);

    ReadCommandId(setActorList, reader);
	
    setActorList->numActors = reader->ReadUInt32();
    setActorList->actorList.reserve(setActorList->numActors);
    for (uint32_t i = 0; i < setActorList->numActors; i++) {
	ActorEntry entry;

	entry.id = reader->ReadUInt16();
	entry.pos.x = reader->ReadInt16();
	entry.pos.y = reader->ReadInt16();
	entry.pos.z = reader->ReadInt16();
	entry.rot.x = reader->ReadInt16();
	entry.rot.y = reader->ReadInt16();
	entry.rot.z = reader->ReadInt16();
	entry.params = reader->ReadUInt16();

        setActorList->actorList.push_back(entry);
    }
}

} // namespace Ship
