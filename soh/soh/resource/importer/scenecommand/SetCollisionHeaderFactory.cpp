#include "soh/resource/importer/scenecommand/SetCollisionHeaderFactory.h"
#include "soh/resource/type/scenecommand/SetCollisionHeader.h"
#include "libultraship/bridge.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetCollisionHeaderFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetCollisionHeader>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetCollisionHeaderFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetCollisionHeader with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetCollisionHeaderFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetCollisionHeader> setCollisionHeader = std::static_pointer_cast<SetCollisionHeader>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setCollisionHeader);

	ReadCommandId(setCollisionHeader, reader);
	
	setCollisionHeader->fileName = reader->ReadString();
	setCollisionHeader->collisionHeader = std::static_pointer_cast<CollisionHeader>(LoadResource(setCollisionHeader->fileName.c_str()));
}

} // namespace Ship
