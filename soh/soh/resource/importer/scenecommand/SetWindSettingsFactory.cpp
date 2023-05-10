#include "soh/resource/importer/scenecommand/SetWindSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetWindSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetWindSettingsFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                               std::shared_ptr<ResourceInitData> initData,
                                                               std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetWindSettings>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetWindSettingsFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetWindSettings with version {}", resource->InitData->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::SetWindSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
    std::shared_ptr<SetWindSettings> setWind = std::static_pointer_cast<SetWindSettings>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setWind);

    ReadCommandId(setWind, reader);
    
    setWind->settings.windWest = reader->ReadInt8();
    setWind->settings.windVertical = reader->ReadInt8();
    setWind->settings.windSouth = reader->ReadInt8();
    setWind->settings.windSpeed = reader->ReadUByte();
}

} // namespace Ship
