#include "soh/resource/importer/scenecommand/SetSkyboxModifierFactory.h"
#include "soh/resource/type/scenecommand/SetSkyboxModifier.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetSkyboxModifierFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                 std::shared_ptr<LUS::BinaryReader> reader) {
    auto setSkyboxModifier = std::make_shared<SetSkyboxModifier>(initData);

	ReadCommandId(setSkyboxModifier, reader);
	
    setSkyboxModifier->modifier.skyboxDisabled = reader->ReadInt8();
    setSkyboxModifier->modifier.sunMoonDisabled = reader->ReadInt8();

    //LogSkyboxModifierAsXML(setSkyboxModifier);

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

void LogSkyboxModifierAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetSkyboxModifier> setSkyboxModifier = std::static_pointer_cast<SetSkyboxModifier>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetSkyboxModifier");
    doc.InsertFirstChild(root);

    root->SetAttribute("SkyboxDisabled", setSkyboxModifier->modifier.skyboxDisabled);
    root->SetAttribute("SunMoonDisabled", setSkyboxModifier->modifier.sunMoonDisabled);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
