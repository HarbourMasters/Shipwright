#include "soh/resource/importer/scenecommand/SetSoundSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetSoundSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<ShipDK::IResource> SetSoundSettingsFactory::ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData,
                                                                std::shared_ptr<ShipDK::BinaryReader> reader) {
    auto setSoundSettings = std::make_shared<SetSoundSettings>(initData);

    ReadCommandId(setSoundSettings, reader);
	
    setSoundSettings->settings.reverb = reader->ReadInt8();
    setSoundSettings->settings.natureAmbienceId = reader->ReadInt8();
    setSoundSettings->settings.seqId = reader->ReadInt8();

    return setSoundSettings;
}
} // namespace SOH
