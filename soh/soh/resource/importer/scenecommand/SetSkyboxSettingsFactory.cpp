#include "soh/resource/importer/scenecommand/SetSkyboxSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetSkyboxSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetSkyboxSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                 std::shared_ptr<LUS::BinaryReader> reader) {
    auto setSkyboxSettings = std::make_shared<SetSkyboxSettings>(initData);

    ReadCommandId(setSkyboxSettings, reader);
	
    setSkyboxSettings->settings.unk = reader->ReadInt8();
    setSkyboxSettings->settings.skyboxId = reader->ReadInt8();
    setSkyboxSettings->settings.weather = reader->ReadInt8();
    setSkyboxSettings->settings.indoors = reader->ReadInt8();

    //LogSkyboxSettingsAsXML(setSkyboxSettings);

    return setSkyboxSettings;
}

std::shared_ptr<LUS::IResource> SetSkyboxSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setSkyboxSettings = std::make_shared<SetSkyboxSettings>(initData);

    setSkyboxSettings->cmdId = SceneCommandID::SetSkyboxSettings;

    setSkyboxSettings->settings.unk = reader->IntAttribute("Unknown");
    setSkyboxSettings->settings.skyboxId = reader->IntAttribute("SkyboxId");
    setSkyboxSettings->settings.weather = reader->IntAttribute("Weather");
    setSkyboxSettings->settings.indoors = reader->IntAttribute("Indoors");

    return setSkyboxSettings;
}

void LogSkyboxSettingsAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetSkyboxSettings> setSkyboxSettings = std::static_pointer_cast<SetSkyboxSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetSkyboxSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("Unknown", setSkyboxSettings->settings.unk);
    root->SetAttribute("SkyboxId", setSkyboxSettings->settings.skyboxId);
    root->SetAttribute("Weather", setSkyboxSettings->settings.weather);
    root->SetAttribute("Indoors", setSkyboxSettings->settings.indoors);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
