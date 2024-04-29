#include "soh/resource/importer/scenecommand/SetWindSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetWindSettings.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetWindSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setWind = std::make_shared<SetWindSettings>(initData);

    ReadCommandId(setWind, reader);
    
    setWind->settings.windWest = reader->ReadInt8();
    setWind->settings.windVertical = reader->ReadInt8();
    setWind->settings.windSouth = reader->ReadInt8();
    setWind->settings.windSpeed = reader->ReadUByte();

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogWindSettingsAsXML(setWind);
    }

    return setWind;
}

std::shared_ptr<Ship::IResource> SetWindSettingsFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setWind = std::make_shared<SetWindSettings>(initData);

    setWind->cmdId = SceneCommandID::SetWind;

    setWind->settings.windWest = reader->IntAttribute("WindWest");
    setWind->settings.windVertical = reader->IntAttribute("WindVertical");
    setWind->settings.windSouth = reader->IntAttribute("WindSouth");
    setWind->settings.windSpeed = reader->IntAttribute("WindSpeed");

    return setWind;
}
} // namespace SOH
