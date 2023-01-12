#include "soh/resource/importer/scenecommand/SetTimeSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetTimeSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetTimeSettingsFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetTimeSettings>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	switch (version)
	{
	case 0:
		factory = std::make_shared<SetTimeSettingsFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetTimeSettings with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetTimeSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetTimeSettings> setTimeSettings = std::static_pointer_cast<SetTimeSettings>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setTimeSettings);

	ReadCommandId(setTimeSettings, reader);
	
	setTimeSettings->settings.hour = reader->ReadInt8();
	setTimeSettings->settings.minute = reader->ReadInt8();
	setTimeSettings->settings.timeIncrement = reader->ReadInt8();
}

} // namespace Ship
