#include "soh/resource/importer/scenecommand/SetSoundSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetSoundSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetSoundSettingsFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetSoundSettings>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	switch (version)
	{
	case 0:
		factory = std::make_shared<SetSoundSettingsFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetSoundSettings with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetSoundSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetSoundSettings> setSoundSettings = std::static_pointer_cast<SetSoundSettings>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setSoundSettings);

	ReadCommandId(setSoundSettings, reader);
	
    setSoundSettings->settings.reverb = reader->ReadInt8();
    setSoundSettings->settings.natureAmbienceId = reader->ReadInt8();
    setSoundSettings->settings.seqId = reader->ReadInt8();
}

} // namespace Ship
