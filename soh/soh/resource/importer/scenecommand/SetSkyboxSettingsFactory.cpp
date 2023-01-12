#include "resource/importer/SetSkyboxSettingsFactory.h"
#include "resource/type/SetSkyboxSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetSkyboxSettingsFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetSkyboxSettings>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetSkyboxSettingsFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetSkyboxSettings with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetSkyboxSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetSkyboxSettings> setSkyboxSettings = std::static_pointer_cast<SetSkyboxSettings>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setSkyboxSettings);

	ReadCommandId(setSkyboxSettings, reader);
	
    setSkyboxModifier->settings.unk = reader->ReadUByte();
	setSkyboxModifier->settings.skyboxId = reader->ReadUByte();
    setSkyboxModifier->settings.weather = reader->ReadUByte();
    setSkyboxModifier->settings.indoors = reader->ReadUByte();
}

} // namespace Ship
