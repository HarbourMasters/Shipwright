#include "2s2h/resource/importer/scenecommand/SetLightingSettingsFactory.h"
#include "2s2h/resource/type/scenecommand/SetLightingSettings.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetLightingSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                         std::shared_ptr<Ship::BinaryReader> reader) {
    auto setLightingSettings = std::make_shared<SetLightingSettings>(initData);

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

    return setLightingSettings;
}
} // namespace SOH
