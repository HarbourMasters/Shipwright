#include "soh/resource/importer/scenecommand/EndMarkerFactory.h"
#include "soh/resource/type/scenecommand/EndMarker.h"
#include "spdlog/spdlog.h"


std::shared_ptr<LUS::IResource>
EndMarkerFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<EndMarker>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	factory = std::make_shared<EndMarkerFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load EndMarker with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::EndMarkerFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<LUS::IResource> resource)
{
    std::shared_ptr<EndMarker> endMarker = std::static_pointer_cast<EndMarker>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, endMarker);

    ReadCommandId(endMarker, reader);
	
    // This has no data.
}


