#include "resource/importer/SetEntranceListFactory.h"
#include "resource/type/SetEntranceList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetEntranceListFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetEntranceListList>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetEntranceListListFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetEntranceListList with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetEntranceListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetEntranceListList> setEntranceList = std::static_pointer_cast<SetEntranceList>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	ReadCommandId(setEntranceList, reader);
	
    setEntranceList->numEntrances = reader->ReadUInt32();
    setEntranceList->rooms.reserve(setEntranceList->numEntrances);
    for (uint32_t i = 0; i < setEntranceList->numExits; i++) {
		EntrancyEntry entranceEntry;

		entranceEntry->spawn = reader->ReadUByte();
		entranceEntry->room = reader->ReadUByte();
		
        setEntranceList->exits.push_back(room);
    }
}

} // namespace Ship
