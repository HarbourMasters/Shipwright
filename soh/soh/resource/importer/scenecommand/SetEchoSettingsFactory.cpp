#include "resource/importer/SetEchoSettingsFactory.h"
#include "resource/type/SetEchoSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetEchoSettingsFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetEchoSettings>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetEchoSettingsFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetEchoSettings with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetEchoSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetEchoSettings> setEchoSettings = std::static_pointer_cast<SetEchoSettings>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setEchoSettings);

	ReadCommandId(setEchoSettings, reader);
	
    setEchoSettings->settings.echo = read->ReadInt8();
}

} // namespace Ship
