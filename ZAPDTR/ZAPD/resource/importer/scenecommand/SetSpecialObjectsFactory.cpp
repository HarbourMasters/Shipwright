#include "resource/importer/SetSpecialObjectsFactory.h"
#include "resource/type/SetSpecialObjects.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetSpecialObjectsFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetSpecialObjects>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetSpecialObjectsFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetSpecialObjects with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetSpecialObjectsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetSpecialObjects> setSpecialObjects = std::static_pointer_cast<SetSpecialObjects>(resource);
	ResourceFile::ParseFileBinary(reader, setSpecialObjects);

	ReadCommandId(setSpecialObjects, reader);
	
	setSpecialObjects->specialObjects.elfMessage = reader->ReadInt8();
	setSpecialObjects->specialObjects.globalObject = reader->ReadInt16();
}

} // namespace Ship
