#include "soh/resource/importer/scenecommand/SetTransitionActorListFactory.h"
#include "soh/resource/type/scenecommand/SetTransitionActorList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetTransitionActorListFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                                      std::shared_ptr<ResourceInitData> initData,
                                                                      std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetTransitionActorList>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetTransitionActorListFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetTransitionActorList with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::SetTransitionActorListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
    std::shared_ptr<SetTransitionActorList> setTransitionActorList = std::static_pointer_cast<SetTransitionActorList>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setTransitionActorList);

    ReadCommandId(setTransitionActorList, reader);
	
    setTransitionActorList->numTransitionActors = reader->ReadUInt32();
    setTransitionActorList->transitionActorList.reserve(setTransitionActorList->numTransitionActors);
    for (uint32_t i = 0; i < setTransitionActorList->numTransitionActors; i++) {
        TransitionActorEntry entry;

        entry.sides[0].room = reader->ReadUByte();
        entry.sides[0].effects = reader->ReadUByte();
        entry.sides[1].room = reader->ReadUByte();
        entry.sides[1].effects = reader->ReadUByte();
        entry.id = reader->ReadInt16();
        entry.pos.x = reader->ReadInt16();
        entry.pos.y = reader->ReadInt16();
        entry.pos.z = reader->ReadInt16();
        entry.rotY = reader->ReadInt16();
        entry.params = reader->ReadUInt16();

        setTransitionActorList->transitionActorList.push_back(entry);
    }
}

} // namespace Ship
