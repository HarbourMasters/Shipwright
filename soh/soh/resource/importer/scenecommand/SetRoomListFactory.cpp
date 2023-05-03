#include "soh/resource/importer/scenecommand/SetRoomListFactory.h"
#include "soh/resource/type/scenecommand/SetRoomList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetRoomListFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                           std::shared_ptr<ResourceInitData> initData,
                                                           std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetRoomList>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<SetRoomListFactoryV0>();
	break;
    }

    if (factory == nullptr)
    {
	SPDLOG_ERROR("Failed to load SetRoomList with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::SetRoomListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
    std::shared_ptr<SetRoomList> setRoomList = std::static_pointer_cast<SetRoomList>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setRoomList);

    ReadCommandId(setRoomList, reader);
	
    setRoomList->numRooms = reader->ReadInt32();
    setRoomList->rooms.reserve(setRoomList->numRooms);
    for (uint32_t i = 0; i < setRoomList->numRooms; i++) {
	RomFile room;

	setRoomList->fileNames.push_back(reader->ReadString());

	room.fileName = (char*)setRoomList->fileNames.back().c_str();
	room.vromStart = reader->ReadInt32();
	room.vromEnd = reader->ReadInt32();
		
        setRoomList->rooms.push_back(room);
    }
}

} // namespace Ship
