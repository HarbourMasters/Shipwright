#include "resource/importer/SetLightingsSettingsFactory.h"
#include "resource/type/SetLightingsSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetLightingsSettingsFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetLightingsSettings>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetLightingsSettingsFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetLightingsSettings with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetLightingsSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetLightingsSettings> setLightingsSettings = std::static_pointer_cast<SetLightingsSettings>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setLightingsSettings);

	ReadCommandId(setLightingsSettings, reader);

	uint32_t count = reader->ReadInt32();
    setLightingsSettings->settings.reserve(count);

	for (uint32_t i = 0; i < count; i++) {
		EnvLightSettings lightSettings;
        lightSettings.ambientColor[0] = reader->ReadInt8();
        lightSettings.ambientColor[1] = reader->ReadInt8();
        lightSettings.ambientColor[2] = reader->ReadInt8();

        lightSettings.light1Color[0] = reader->ReadInt8();
        lightSettings.light1Color[1] = reader->ReadInt8();
        lightSettings.light1Color[2] = reader->ReadInt8();

        lightSettings.light1Dir[0] = reader->ReadInt8();
        lightSettings.light1Dir[1] = reader->ReadInt8();
        lightSettings.light1Dir[2] = reader->ReadInt8();

        lightSettings.light2Color[0] = reader->ReadInt8();
        lightSettings.light2Color[1] = reader->ReadInt8();
        lightSettings.light2Color[2] = reader->ReadInt8();

        lightSettings.light2Dir[0] = reader->ReadInt8();
        lightSettings.light2Dir[1] = reader->ReadInt8();
        lightSettings.light2Dir[2] = reader->ReadInt8();

        lightSettings.fogColor[0] = reader->ReadInt8();
        lightSettings.fogColor[1] = reader->ReadInt8();
        lightSettings.fogColor[2] = reader->ReadInt8();

        lightSettings.fogNear = reader->ReadInt16();
        lightSettings.fogFar = reader->ReadInt16();
		settings.push_back(lightSettings);
	}
}

} // namespace Ship
