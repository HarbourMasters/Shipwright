#include "soh/resource/importer/scenecommand/SetEchoSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetEchoSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<ShipDK::IResource>
SetEchoSettingsFactory::ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData, std::shared_ptr<ShipDK::BinaryReader> reader) {
    auto setEchoSettings = std::make_shared<SetEchoSettings>(initData);

    ReadCommandId(setEchoSettings, reader);
	
    setEchoSettings->settings.echo = reader->ReadInt8();

    return setEchoSettings;
}
} // namespace SOH
