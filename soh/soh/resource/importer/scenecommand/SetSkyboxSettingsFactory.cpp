#include "soh/resource/importer/scenecommand/SetSkyboxSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetSkyboxSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetSkyboxSettingsFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                                 std::shared_ptr<ResourceInitData> initData,
                                                                 std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetSkyboxSettings>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetSkyboxSettingsFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetSkyboxSettings with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void SetSkyboxSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
    std::shared_ptr<SetSkyboxSettings> setSkyboxSettings = std::static_pointer_cast<SetSkyboxSettings>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setSkyboxSettings);

    ReadCommandId(setSkyboxSettings, reader);
	
    setSkyboxSettings->settings.unk = reader->ReadInt8();
    setSkyboxSettings->settings.skyboxId = reader->ReadInt8();
    setSkyboxSettings->settings.weather = reader->ReadInt8();
    setSkyboxSettings->settings.indoors = reader->ReadInt8();
}

} // namespace Ship
