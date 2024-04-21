#include "soh/resource/importer/scenecommand/SetTimeSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetTimeSettings.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetTimeSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setTimeSettings = std::make_shared<SetTimeSettings>(initData);

    ReadCommandId(setTimeSettings, reader);
    
    setTimeSettings->settings.hour = reader->ReadInt8();
    setTimeSettings->settings.minute = reader->ReadInt8();
    setTimeSettings->settings.timeIncrement = reader->ReadInt8();

    if (CVarGetInteger("gDebugResourceLogging", 0)) {
        LogTimeSettingsAsXML(setTimeSettings);
    }

    return setTimeSettings;
}

std::shared_ptr<LUS::IResource> SetTimeSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setTimeSettings = std::make_shared<SetTimeSettings>(initData);

    setTimeSettings->cmdId = SceneCommandID::SetTimeSettings;

    setTimeSettings->settings.hour = reader->IntAttribute("Hour");
    setTimeSettings->settings.minute = reader->IntAttribute("Minute");
    setTimeSettings->settings.timeIncrement = reader->IntAttribute("TimeIncrement");

    return setTimeSettings;
}
} // namespace SOH
