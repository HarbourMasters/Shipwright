#include "soh/resource/importer/scenecommand/SetSkyboxModifierFactory.h"
#include "soh/resource/type/scenecommand/SetSkyboxModifier.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<ShipDK::IResource> SetSkyboxModifierFactory::ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData,
                                                                 std::shared_ptr<ShipDK::BinaryReader> reader) {
    auto setSkyboxModifier = std::make_shared<SetSkyboxModifier>(initData);

	ReadCommandId(setSkyboxModifier, reader);
	
    setSkyboxModifier->modifier.skyboxDisabled = reader->ReadInt8();
    setSkyboxModifier->modifier.sunMoonDisabled = reader->ReadInt8();

    return setSkyboxModifier;
}
} // namespace SOH
