#include "soh/resource/importer/scenecommand/SetSkyboxModifierFactory.h"
#include "soh/resource/type/scenecommand/SetSkyboxModifier.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetSkyboxModifierFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                 std::shared_ptr<LUS::BinaryReader> reader) {
    auto setSkyboxModifier = std::make_shared<SetSkyboxModifier>(initData);

	ReadCommandId(setSkyboxModifier, reader);
	
    setSkyboxModifier->modifier.skyboxDisabled = reader->ReadInt8();
    setSkyboxModifier->modifier.sunMoonDisabled = reader->ReadInt8();

    if (CVarGetInteger("gDebugResourceLogging", 0)) {
        LogSkyboxModifierAsXML(setSkyboxModifier);
    }

    return setSkyboxModifier;
}

std::shared_ptr<LUS::IResource> SetSkyboxModifierFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setSkyboxModifier = std::make_shared<SetSkyboxModifier>(initData);

    setSkyboxModifier->cmdId = SceneCommandID::SetSkyboxModifier;

    setSkyboxModifier->modifier.skyboxDisabled = reader->IntAttribute("SkyboxDisabled");
    setSkyboxModifier->modifier.sunMoonDisabled = reader->IntAttribute("SunMoonDisabled");

    return setSkyboxModifier;
}
} // namespace SOH
