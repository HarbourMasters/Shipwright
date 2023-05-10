#include "soh/resource/importer/scenecommand/EndMarkerFactory.h"
#include "soh/resource/type/scenecommand/EndMarker.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> EndMarkerFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                         std::shared_ptr<ResourceInitData> initData,
                                                         std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<EndMarker>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<EndMarkerFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load EndMarker with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::EndMarkerFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
    std::shared_ptr<EndMarker> endMarker = std::static_pointer_cast<EndMarker>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, endMarker);

    ReadCommandId(endMarker, reader);
	
    // This has no data.
}

} // namespace Ship
