#include "2s2h/resource/importer/scenecommand/SetSoundSettingsFactory.h"
#include "2s2h/resource/type/scenecommand/SetSoundSettings.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetSoundSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                       std::shared_ptr<Ship::BinaryReader> reader) {
    auto setSoundSettings = std::make_shared<SetSoundSettings>(initData);

    ReadCommandId(setSoundSettings, reader);

    setSoundSettings->settings.reverb = reader->ReadInt8();
    setSoundSettings->settings.natureAmbienceId = reader->ReadInt8();
    setSoundSettings->settings.seqId = reader->ReadInt8();

    return setSoundSettings;
}
} // namespace SOH
