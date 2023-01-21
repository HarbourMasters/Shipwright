#include "soh/resource/importer/scenecommand/SetEchoSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetEchoSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetEchoSettingsFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetEchoSettings>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

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
	
    setEchoSettings->settings.echo = reader->ReadInt8();
}

} // namespace Ship
