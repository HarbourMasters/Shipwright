#include "soh/resource/importer/scenecommand/SetSkyboxSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetSkyboxSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetSkyboxSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                 std::shared_ptr<LUS::BinaryReader> reader) {
    auto setSkyboxSettings = std::make_shared<SetSkyboxSettings>(initData);

    ReadCommandId(setSkyboxSettings, reader);
	
    setSkyboxSettings->settings.unk = reader->ReadInt8();
    setSkyboxSettings->settings.skyboxId = reader->ReadInt8();
    setSkyboxSettings->settings.weather = reader->ReadInt8();
    setSkyboxSettings->settings.indoors = reader->ReadInt8();

    return setSkyboxSettings;
}
} // namespace SOH
