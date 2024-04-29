#include "soh/resource/importer/scenecommand/SetEchoSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetEchoSettings.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetEchoSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setEchoSettings = std::make_shared<SetEchoSettings>(initData);

    ReadCommandId(setEchoSettings, reader);
	
    setEchoSettings->settings.echo = reader->ReadInt8();

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogEchoSettingsAsXML(setEchoSettings);
    }

    return setEchoSettings;
}

std::shared_ptr<LUS::IResource> SetEchoSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setEchoSettings = std::make_shared<SetEchoSettings>(initData);

    setEchoSettings->cmdId = SceneCommandID::SetEchoSettings;

    setEchoSettings->settings.echo = reader->IntAttribute("Echo");

    return setEchoSettings;
}
} // namespace SOH
