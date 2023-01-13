#include "soh/resource/importer/scenecommand/SetPathwaysFactory.h"
#include "soh/resource/type/scenecommand/SetPathways.h"
#include "spdlog/spdlog.h"
#include <libultraship/bridge.h>

namespace Ship {
std::shared_ptr<Resource> SetPathwaysFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetPathways>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	switch (version)
	{
	case 0:
		factory = std::make_shared<SetPathwaysFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetPathways with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetPathwaysFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                 std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetPathways> setPathways = std::static_pointer_cast<SetPathways>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setPathways);

	ReadCommandId(setPathways, reader);
	
    setPathways->numPaths = reader->ReadUInt32();
    setPathways->paths.reserve(setPathways->numPaths);
    for (uint32_t i = 0; i < setPathways->numPaths; i++) {
        std::string pathFileName = reader->ReadString();
        setPathways->paths.push_back(std::static_pointer_cast<Path>(LoadResource(pathFileName.c_str(), true)));
    }
}

} // namespace Ship
