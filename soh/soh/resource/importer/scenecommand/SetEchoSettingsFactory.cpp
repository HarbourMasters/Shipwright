#include "soh/resource/importer/scenecommand/SetEchoSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetEchoSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetEchoSettingsFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                               std::shared_ptr<ResourceInitData> initData,
                                                               std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetEchoSettings>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetEchoSettingsFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetEchoSettings with version {}", resource->InitData->ResourceVersion);
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
