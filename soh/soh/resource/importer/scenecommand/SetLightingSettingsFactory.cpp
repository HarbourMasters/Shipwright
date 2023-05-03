#include "soh/resource/importer/scenecommand/SetLightingSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetLightingSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetLightingSettingsFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                                   std::shared_ptr<ResourceInitData> initData,
                                                                   std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetLightingSettings>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<SetLightingSettingsFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetLightingSettings with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::SetLightingSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
    std::shared_ptr<SetLightingSettings> setLightingSettings = std::static_pointer_cast<SetLightingSettings>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setLightingSettings);

    ReadCommandId(setLightingSettings, reader);

    uint32_t count = reader->ReadInt32();
    setLightingSettings->settings.reserve(count);

    for (uint32_t i = 0; i < count; i++) {
        EnvLightSettings lightSettings;
        lightSettings.ambientColor[0] = reader->ReadInt8();
        lightSettings.ambientColor[1] = reader->ReadInt8();
        lightSettings.ambientColor[2] = reader->ReadInt8();

        lightSettings.light1Dir[0] = reader->ReadInt8();
        lightSettings.light1Dir[1] = reader->ReadInt8();
        lightSettings.light1Dir[2] = reader->ReadInt8();

        lightSettings.light1Color[0] = reader->ReadInt8();
        lightSettings.light1Color[1] = reader->ReadInt8();
        lightSettings.light1Color[2] = reader->ReadInt8();

        lightSettings.light2Dir[0] = reader->ReadInt8();
        lightSettings.light2Dir[1] = reader->ReadInt8();
        lightSettings.light2Dir[2] = reader->ReadInt8();

        lightSettings.light2Color[0] = reader->ReadInt8();
        lightSettings.light2Color[1] = reader->ReadInt8();
        lightSettings.light2Color[2] = reader->ReadInt8();

        lightSettings.fogColor[0] = reader->ReadInt8();
        lightSettings.fogColor[1] = reader->ReadInt8();
        lightSettings.fogColor[2] = reader->ReadInt8();

        lightSettings.fogNear = reader->ReadInt16();
        lightSettings.fogFar = reader->ReadUInt16();
        setLightingSettings->settings.push_back(lightSettings);
    }
}

} // namespace Ship
