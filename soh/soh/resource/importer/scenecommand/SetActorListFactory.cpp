#include "soh/resource/importer/scenecommand/SetActorListFactory.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
#include "spdlog/spdlog.h"


std::shared_ptr<LUS::IResource>
SetActorListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto resource = std::make_shared<SetActorList>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	factory = std::make_shared<SetActorListFactoryV0>();
	break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load SetActorList with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::SetActorListFactoryV0::ParseFileBinary(std::shared_ptr<LUS::BinaryReader> reader,
                                        std::shared_ptr<LUS::IResource> resource) {
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


