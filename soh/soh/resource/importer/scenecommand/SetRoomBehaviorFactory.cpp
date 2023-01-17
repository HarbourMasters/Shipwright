#include "soh/resource/importer/scenecommand/SetRoomBehaviorFactory.h"
#include "soh/resource/type/scenecommand/SetRoomBehavior.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetRoomBehaviorFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetRoomBehavior>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	switch (version)
	{
	case 0:
		factory = std::make_shared<SetRoomBehaviorFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetRoomBehavior with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetRoomBehaviorFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetRoomBehavior> setRoomBehavior = std::static_pointer_cast<SetRoomBehavior>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setRoomBehavior);

	ReadCommandId(setRoomBehavior, reader);
	
    setRoomBehavior->roomBehavior.gameplayFlags = reader->ReadInt8();
    setRoomBehavior->roomBehavior.gameplayFlags2 = reader->ReadInt32();
}

} // namespace Ship
