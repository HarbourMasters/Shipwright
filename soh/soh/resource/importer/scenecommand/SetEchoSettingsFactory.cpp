#include "soh/resource/importer/scenecommand/SetEchoSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetEchoSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetEchoSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setEchoSettings = std::make_shared<SetEchoSettings>(initData);

    ReadCommandId(setEchoSettings, reader);
	
    setEchoSettings->settings.echo = reader->ReadInt8();

    //LogEchoSettingsAsXML(setEchoSettings);

    return setEchoSettings;
}

std::shared_ptr<LUS::IResource> SetEchoSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setEchoSettings = std::make_shared<SetEchoSettings>(initData);

    setEchoSettings->cmdId = SceneCommandID::SetEchoSettings;

    setEchoSettings->settings.echo = reader->IntAttribute("Echo");

    return setEchoSettings;
}

void LogEchoSettingsAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetEchoSettings> setEchoSettings = std::static_pointer_cast<SetEchoSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetEchoSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("Echo", setEchoSettings->settings.echo);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
