#include "resource/importer/SetStartPositionListFactory.h"
#include "resource/type/SetStartPositionList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetStartPositionListFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetStartPositionList>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetStartPositionListFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetStartPositionList with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetStartPositionListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetStartPositionList> setStartPositionList = std::static_pointer_cast<SetStartPositionList>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	ReadCommandId(setStartPositionList, reader);
	
    setStartPositionList->numStartPositions = reader->ReadUInt32();
    setStartPositionList->startPositions.reserve(setStartPositionList->numStartPositions);
    for (uint32_t i = 0; i < setStartPositionList->numExits; i++) {
		ActorEntry entry;

		entry.id = reader->ReadInt16();
		entry.pos.x = reader->ReadInt16();
		entry.pos.y = reader->ReadInt16();
		entry.pos.z = reader->ReadInt16();
		entry.rot.x = reader->ReadInt16();
		entry.rot.y = reader->ReadInt16();
		entry.rot.z = reader->ReadInt16();
		entry.params = reader->ReadInt16();

        setStartPositionList->objects.push_back(entry);
    }
}

} // namespace Ship
