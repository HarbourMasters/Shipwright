#include "resource/importer/SetAlternateHeaderFactory.h"
#include "resource/type/SetAlternateHeader.h"
#include "spdlog/spdlog.h"
#include "resourcebridge.h"

namespace Ship {
std::shared_ptr<Resource> SetAlternateHeaderFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetAlternateHeader>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetAlternateHeaderFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetAlternateHeader with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetAlternateHeaderFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetAlternateHeader> setAlternateHeader = std::static_pointer_cast<SetAlternateHeader>(resource);
	ResourceFile::ParseFileBinary(reader, setAlternateHeader);

	ReadCommandId(setAlternateHeader, reader);
	
    setAlternateHeader->numHeaders = reader->ReadUInt32();
    setAlternateHeader->headers.reserve(setAlternateHeader->numRooms);
    for (uint32_t i = 0; i < SetAlternateHeader->numExits; i++) {
		headers.push_back(LoadResource(reader->ReadString()));
    }
}

} // namespace Ship
