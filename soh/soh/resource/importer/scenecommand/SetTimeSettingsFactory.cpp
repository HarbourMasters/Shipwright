#include "soh/resource/importer/scenecommand/SetTimeSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetTimeSettings.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetTimeSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setTimeSettings = std::make_shared<SetTimeSettings>(initData);

    ReadCommandId(setTimeSettings, reader);
    
    setTimeSettings->settings.hour = reader->ReadInt8();
    setTimeSettings->settings.minute = reader->ReadInt8();
    setTimeSettings->settings.timeIncrement = reader->ReadInt8();

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogTimeSettingsAsXML(setTimeSettings);
    }

    return setTimeSettings;
}

std::shared_ptr<Ship::IResource> SetTimeSettingsFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setTimeSettings = std::make_shared<SetTimeSettings>(initData);

    setTimeSettings->cmdId = SceneCommandID::SetTimeSettings;

    setTimeSettings->settings.hour = reader->IntAttribute("Hour");
    setTimeSettings->settings.minute = reader->IntAttribute("Minute");
    setTimeSettings->settings.timeIncrement = reader->IntAttribute("TimeIncrement");

    return setTimeSettings;
}
} // namespace SOH
