#include "soh/resource/importer/scenecommand/SetTimeSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetTimeSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetTimeSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setTimeSettings = std::make_shared<SetTimeSettings>(initData);

    ReadCommandId(setTimeSettings, reader);
    
    setTimeSettings->settings.hour = reader->ReadInt8();
    setTimeSettings->settings.minute = reader->ReadInt8();
    setTimeSettings->settings.timeIncrement = reader->ReadInt8();

    return setTimeSettings;
}
} // namespace SOH
