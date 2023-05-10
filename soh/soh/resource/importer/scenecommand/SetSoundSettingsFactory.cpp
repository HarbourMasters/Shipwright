#include "soh/resource/importer/scenecommand/SetSoundSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetSoundSettings.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetSoundSettingsFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                                std::shared_ptr<ResourceInitData> initData,
                                                                std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetSoundSettings>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetSoundSettingsFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetSoundSettings with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::SetSoundSettingsFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
    std::shared_ptr<SetSoundSettings> setSoundSettings = std::static_pointer_cast<SetSoundSettings>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setSoundSettings);

    ReadCommandId(setSoundSettings, reader);
	
    setSoundSettings->settings.reverb = reader->ReadInt8();
    setSoundSettings->settings.natureAmbienceId = reader->ReadInt8();
    setSoundSettings->settings.seqId = reader->ReadInt8();
}

} // namespace Ship
