#include "soh/resource/importer/scenecommand/SetSkyboxModifierFactory.h"
#include "soh/resource/type/scenecommand/SetSkyboxModifier.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetSkyboxModifierFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                                 std::shared_ptr<ResourceInitData> initData,
                                                                 std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetSkyboxModifier>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<SetSkyboxModifierFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetSkyboxModifier with version {}", resource->InitData->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::SetSkyboxModifierFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource) {
	std::shared_ptr<SetSkyboxModifier> setSkyboxModifier = std::static_pointer_cast<SetSkyboxModifier>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setSkyboxModifier);

	ReadCommandId(setSkyboxModifier, reader);
	
    setSkyboxModifier->modifier.skyboxDisabled = reader->ReadInt8();
    setSkyboxModifier->modifier.sunMoonDisabled = reader->ReadInt8();
}

} // namespace Ship
