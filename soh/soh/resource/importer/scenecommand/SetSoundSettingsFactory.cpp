#include "resource/importer/SetSoundSettingsFactory.h"
#include "resource/type/SetSoundSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetSoundSettingsFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetSoundSettings>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
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
	
	setSoundSettings->settings.seqId = reader->ReadUByte();
    setSoundSettings->settings.natureAmbienceId = reader->ReadUByte();
    setSoundSettings->settings.reverb = reader->ReadUByte();
}

} // namespace Ship
