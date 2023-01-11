#include "resource/importer/SetTransitionActorListFactory.h"
#include "resource/type/SetTransitionActorList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetTransitionActorListFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetTransitionActorList>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetTransitionActorListFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetTransitionActorList with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetTransitionActorListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetTransitionActorList> setTransitionActorList = std::static_pointer_cast<SetTransitionActorList>(resource);
	ResourceFile::ParseFileBinary(reader, setTransitionActorList);

	ReadCommandId(SetTransitionActorList, reader);
	
    setTransitionActorList->numTransitionActors = reader->ReadUInt32();
    setTransitionActorList->transitionActorList.reserve(setTransitionActorList->numTransitionActors);
    for (uint32_t i = 0; i < setTransitionActorList->numExits; i++) {
		TransitionActorEntry entry;

        entry.sides[0].room = reader->ReadInt8();
        entry.sides[0].effects = reader->ReadInt8();
        entry.sides[1].room = reader->ReadInt8();
        entry.sides[1].effects = reader->ReadInt8();
        entry.id = reader->ReadInt16();
        entry.pos.x = reader->ReadInt16();
        entry.pos.y = reader->ReadInt16();
        entry.pos.z = reader->ReadInt16();
        entry.rotY = reader->ReadInt16();
        entry.params = reader->ReadInt16();

        setTransitionActorList->transitionActorList.push_back(entry);
    }
}

} // namespace Ship
