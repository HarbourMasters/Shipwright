#include "resource/importer/SetRoomListFactory.h"
#include "resource/type/SetRoomList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetRoomListFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetRoomList>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetRoomListFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetRoomList with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetRoomListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetRoomList> setRoomList = std::static_pointer_cast<SetRoomList>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setRoomList);

	ReadCommandId(setRoomList, reader);
	
    setRoomList->numRooms = reader->ReadUInt32();
    setRoomList->rooms.reserve(setRoomList->numRooms);
    for (uint32_t i = 0; i < setRoomList->numExits; i++) {
		RomFile room;

		setRoomList->fileNames.push_back(reader->ReadString());

		room.fileName = setRoomList->fileNames.back().c_str();
		room.vromStart = reader->ReadUint32();
		room.vromEnd = reader->ReadUint32();
		
        setRoomList->rooms.push_back(room);
    }
}

} // namespace Ship
