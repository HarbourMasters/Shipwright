#include "2s2h/resource/importer/scenecommand/SetEchoSettingsFactory.h"
#include "2s2h/resource/type/scenecommand/SetEchoSettings.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetEchoSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                      std::shared_ptr<Ship::BinaryReader> reader) {
    auto setEchoSettings = std::make_shared<SetEchoSettings>(initData);

    ReadCommandId(setEchoSettings, reader);

    setEchoSettings->settings.echo = reader->ReadInt8();

    return setEchoSettings;
}
} // namespace SOH
