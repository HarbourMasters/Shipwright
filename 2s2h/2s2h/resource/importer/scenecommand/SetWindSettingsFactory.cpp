#include "2s2h/resource/importer/scenecommand/SetWindSettingsFactory.h"
#include "2s2h/resource/type/scenecommand/SetWindSettings.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetWindSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                      std::shared_ptr<Ship::BinaryReader> reader) {
    auto setWind = std::make_shared<SetWindSettings>(initData);

    ReadCommandId(setWind, reader);

    setWind->settings.windWest = reader->ReadInt8();
    setWind->settings.windVertical = reader->ReadInt8();
    setWind->settings.windSouth = reader->ReadInt8();
    setWind->settings.windSpeed = reader->ReadUByte();

    return setWind;
}

} // namespace SOH
