#include "soh/resource/importer/scenecommand/SetAlternateHeadersFactory.h"
#include "soh/resource/type/scenecommand/SetAlternateHeaders.h"
#include "spdlog/spdlog.h"
#include "libultraship/bridge.h"

namespace Ship {
std::shared_ptr<Resource> SetAlternateHeadersFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetAlternateHeaders>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	switch (version)
	{
	case 0:
		factory = std::make_shared<SetAlternateHeadersFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetAlternateHeaders with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetAlternateHeadersFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetAlternateHeaders> setAlternateHeaders = std::static_pointer_cast<SetAlternateHeaders>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setAlternateHeaders);

	ReadCommandId(setAlternateHeaders, reader);
	
    setAlternateHeaders->numHeaders = reader->ReadUInt32();
    setAlternateHeaders->headers.reserve(setAlternateHeaders->numHeaders);
    for (uint32_t i = 0; i < setAlternateHeaders->numHeaders; i++) {
		auto headerName = reader->ReadString();
		if (!headerName.empty()) {
			setAlternateHeaders->headers.push_back(std::static_pointer_cast<Ship::Scene>(LoadResource(headerName.c_str(), true)));
		}
    }
}

} // namespace Ship
