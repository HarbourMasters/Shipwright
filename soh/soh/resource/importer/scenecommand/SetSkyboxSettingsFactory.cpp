#include "soh/resource/importer/scenecommand/SetSkyboxSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetSkyboxSettings.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
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

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogSkyboxSettingsAsXML(setSkyboxSettings);
    }

    return setSkyboxSettings;
}

std::shared_ptr<LUS::IResource> SetSkyboxSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setSkyboxSettings = std::make_shared<SetSkyboxSettings>(initData);

    setSkyboxSettings->cmdId = SceneCommandID::SetSkyboxSettings;

    setSkyboxSettings->settings.unk = reader->IntAttribute("Unknown");
    setSkyboxSettings->settings.skyboxId = reader->IntAttribute("SkyboxId");
    setSkyboxSettings->settings.weather = reader->IntAttribute("Weather");
    setSkyboxSettings->settings.indoors = reader->IntAttribute("Indoors");

    return setSkyboxSettings;
}
} // namespace SOH
