#include "resource/importer/SetActorListFactory.h"
#include "resource/type/SetActorList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetActorListFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetActorList>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetActorListFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetActorList with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetActorListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetActorList> setActorList = std::static_pointer_cast<SetActorList>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	ReadCommandId(setActorList, reader);
	
    setActorList->numActors = reader->ReadUInt32();
    setActorList->actorList.reserve(SetActorList->numActors);
    for (uint32_t i = 0; i < setActorList->numExits; i++) {
		ActorEntry entry;

		entry.id = reader->ReadInt16();
		entry.pos.x = reader->ReadInt16();
		entry.pos.y = reader->ReadInt16();
		entry.pos.z = reader->ReadInt16();
		entry.rot.x = reader->ReadInt16();
		entry.rot.y = reader->ReadInt16();
		entry.rot.z = reader->ReadInt16();
		entry.params = reader->ReadInt16();

        setActorList->actorList.push_back(entry);
    }
}

} // namespace Ship
