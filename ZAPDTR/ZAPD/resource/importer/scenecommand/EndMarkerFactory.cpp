#include "resource/importer/EndMarkerFactory.h"
#include "resource/type/EndMarker.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> EndMarkerFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<EndMarker>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<EndMarkerFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load EndMarker with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::EndMarkerFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<EndMarker> EndMarker = std::static_pointer_cast<EndMarker>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	ReadCommandId(EndMarker, reader);
	
    // This has no data.
}

} // namespace Ship
