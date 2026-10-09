#include "2s2h/resource/importer/scenecommand/SetSkyboxSettingsFactory.h"
#include "2s2h/resource/type/scenecommand/SetSkyboxSettings.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetSkyboxSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                       std::shared_ptr<Ship::BinaryReader> reader) {
    auto setSkyboxSettings = std::make_shared<SetSkyboxSettings>(initData);

    ReadCommandId(setSkyboxSettings, reader);

    setSkyboxSettings->settings.unk = reader->ReadInt8();
    setSkyboxSettings->settings.skyboxId = reader->ReadInt8();
    setSkyboxSettings->settings.weather = reader->ReadInt8();
    setSkyboxSettings->settings.indoors = reader->ReadInt8();

    return setSkyboxSettings;
}
} // namespace SOH
