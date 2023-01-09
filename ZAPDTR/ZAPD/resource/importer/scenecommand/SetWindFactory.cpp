#include "resource/importer/SetWindFactory.h"
#include "resource/type/SetWind.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetWindFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<Scene>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetWindFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetWind with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetWindFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetWind> setWind = std::static_pointer_cast<SetWind>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	ReadCommandId(setWind, reader);
	
	setWind->settings.windWest = reader->ReadInt8();
    setWind->settings.windVertical = reader->ReadInt8();
    setWind->settings.windSouth = reader->ReadInt8();
    setWind->settings.clothFlappingStrength = reader->ReadInt8();
}

} // namespace Ship
