#include "soh/resource/importer/scenecommand/SetWindSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetWindSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetWindSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setWind = std::make_shared<SetWindSettings>(initData);

    ReadCommandId(setWind, reader);
    
    setWind->settings.windWest = reader->ReadInt8();
    setWind->settings.windVertical = reader->ReadInt8();
    setWind->settings.windSouth = reader->ReadInt8();
    setWind->settings.windSpeed = reader->ReadUByte();

    //LogWindSettingsAsXML(setWind);

    return setWind;
}

std::shared_ptr<LUS::IResource> SetWindSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setWind = std::make_shared<SetWindSettings>(initData);

    setWind->cmdId = SceneCommandID::SetWind;

    setWind->settings.windWest = reader->IntAttribute("WindWest");
    setWind->settings.windVertical = reader->IntAttribute("WindVertical");
    setWind->settings.windSouth = reader->IntAttribute("WindSouth");
    setWind->settings.windSpeed = reader->IntAttribute("WindSpeed");

    return setWind;
}

void LogWindSettingsAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetWindSettings> setWindSettings = std::static_pointer_cast<SetWindSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetWindSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("WindWest", setWindSettings->settings.windWest);
    root->SetAttribute("WindVertical", setWindSettings->settings.windVertical);
    root->SetAttribute("WindSouth", setWindSettings->settings.windSouth);
    root->SetAttribute("WindSpeed", setWindSettings->settings.windSpeed);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
