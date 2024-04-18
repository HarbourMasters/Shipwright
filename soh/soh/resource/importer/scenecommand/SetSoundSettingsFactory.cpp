#include "soh/resource/importer/scenecommand/SetSoundSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetSoundSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetSoundSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                std::shared_ptr<LUS::BinaryReader> reader) {
    auto setSoundSettings = std::make_shared<SetSoundSettings>(initData);

    ReadCommandId(setSoundSettings, reader);
	
    setSoundSettings->settings.reverb = reader->ReadInt8();
    setSoundSettings->settings.natureAmbienceId = reader->ReadInt8();
    setSoundSettings->settings.seqId = reader->ReadInt8();

    //LogSoundSettingsAsXML(setSoundSettings);

    return setSoundSettings;
}

std::shared_ptr<LUS::IResource> SetSoundSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setSoundSettings = std::make_shared<SetSoundSettings>(initData);

    setSoundSettings->cmdId = SceneCommandID::SetSoundSettings;

    setSoundSettings->settings.reverb = reader->IntAttribute("Reverb");
    setSoundSettings->settings.natureAmbienceId = reader->IntAttribute("NatureAmbienceId");
    setSoundSettings->settings.seqId = reader->IntAttribute("SeqId");

    return setSoundSettings;
}

void LogSoundSettingsAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetSoundSettings> setSoundSettings = std::static_pointer_cast<SetSoundSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetSoundSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("Reverb", setSoundSettings->settings.reverb);
    root->SetAttribute("NatureAmbienceId", setSoundSettings->settings.natureAmbienceId);
    root->SetAttribute("SeqId", setSoundSettings->settings.seqId);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
