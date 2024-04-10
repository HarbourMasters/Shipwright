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

    return setWind;
}
} // namespace SOH
